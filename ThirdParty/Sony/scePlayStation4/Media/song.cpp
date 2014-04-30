#include "song.h"
#include "../allocator.h"
#include "stream.h"
#include "audio_decoder.h"
#include "at9/audio_decoder_at9.h"
#include "m4aac/audio_decoder_m4aac.h"
#include "mp3/audio_decoder_mp3.h"

#include <assert.h>
#include <libsysmodule.h>
#include <audiodec.h>
#include <audioout.h>
#include <sceerror.h>

using namespace Media;

namespace {
	const uint32_t stackSize = 512 * 1024;
	const uint32_t outputStreamSize = 128 * 1024;
	const uint32_t outputStreamGrain = 256;

	enum EventFlags : uint64_t
	{
		Loaded  = 1 << 0,
		Decoder = 1 << 1,
		Output  = 1 << 2,

		AllDone = Decoder | Output
	};
}

// Private state hidden from public interface
struct Media::SongState
{
	char* fileName;
	SongFinishedHandler onSongFinished;

	SceKernelEventFlag eventFlag;
	ScePthreadBarrier barrier;
	ScePthread threadDecode;
	ScePthread threadOutput;

	InputStream* inputStream;
	AudioOutputStream* outputStream;
	AudioDecoder* decoder;

	bool isPaused;
	bool isRunning;
	bool isLooping;
};

void* decodeMain(void* arg)
{
	auto ret = 0;
	auto state = (Media::SongState*)arg;
	assert(state != NULL);
	assert(state->inputStream == NULL);
	assert(state->decoder == NULL);
	assert(state->outputStream == NULL);

	sceKernelWaitEventFlag(state->eventFlag, EventFlags::Loaded, SCE_KERNEL_EVF_WAITMODE_AND, 0, 0);

	if (state->inputStream != NULL)
		return false;

	assert(state->fileName != NULL);

	state->inputStream = new InputStream();
	assert(state->inputStream);

	ret = state->inputStream->open(state->fileName, "rb");
	assert(ret >= 0);

	state->inputStream->setIsEmpty(false);

	//_state->decoder = new AudioDecoderM4aac(_state->inputStream);
	AudioDecoderTrickPlayPoint point;
	memset(&point, 0, sizeof(AudioDecoderTrickPlayPoint));
	state->decoder = new AudioDecoderAt9(state->inputStream, point, &ret);
	assert(state->decoder);

	// Signal we're loaded and wait for the sync point.
	sceKernelSetEventFlag(state->eventFlag, EventFlags::Decoder);
	scePthreadBarrierWait(&state->barrier);

	while (state->isRunning)
	{
		if (state->inputStream->isEmpty())
		{
			if (state->isLooping)
			{
				state->inputStream->input(NULL, 0, 0, SEEK_SET);
				state->inputStream->setIsEmpty(false);
				state->decoder->restart(state->inputStream);
			}
			else
			{
				break;
			}
		}

		if (state->isPaused)
			continue;

		auto ret = state->decoder->decodeNormal(state->inputStream, state->outputStream, NULL);

		if (ret < 0)
		{
			printf("ERROR: MusicPlayer decode failed: 0x%08X\n", ret);
			goto term;
		}
	}

	state->outputStream->end();

term:
	if (ret < 0)
	{
		sceKernelSetEventFlag(state->eventFlag, EventFlags::Decoder);
		scePthreadBarrierWait(&state->barrier);
	}

	sceKernelClearEventFlag(state->eventFlag, EventFlags::Decoder);
	scePthreadBarrierWait(&state->barrier);

	if (state->decoder != NULL)
	{
		delete state->decoder;
		state->decoder = NULL;
	}

	if (state->inputStream != NULL)
	{
		delete state->inputStream;
		state->inputStream = NULL;
	}

	if (state->onSongFinished != NULL)
	{
		state->onSongFinished();
		state->onSongFinished = NULL;
	}

	return arg;
}

void* outputMain(void* arg)
{
	auto ret = 0;
	auto state = (Media::SongState*)arg;
	assert(state != NULL);
	assert(state->outputStream == NULL);

	sceKernelWaitEventFlag(state->eventFlag, EventFlags::Decoder, SCE_KERNEL_EVF_WAITMODE_AND, 0, 0);
	assert(state->decoder != NULL);

	state->outputStream = new AudioOutputStream(outputStreamSize, sizeof(int16_t) * AUDIO_STEREO * outputStreamGrain);
	assert(state->outputStream);

	ret = state->outputStream->open(outputStreamGrain, state->decoder->sampleRate(), AudioOutputSystem::param(state->decoder->numChannels()));
	assert(ret >= 0);

	sceKernelSetEventFlag(state->eventFlag, EventFlags::Output);
	scePthreadBarrierWait(&state->barrier);

	while(state->isRunning)
	{
		//if (state->isPaused)
		//	continue;

		if (state->inputStream->isEmpty() && state->outputStream->isEmpty())
			break;

		auto ret = state->outputStream->output();
		if (ret < 0)
		{
			printf("ERROR: MusicPlayer output failed: 0x%08X\n", ret);
			goto term;
		}
	}

term:
	if (ret < 0)
	{
		sceKernelSetEventFlag(state->eventFlag, EventFlags::Output);
		scePthreadBarrierWait(&state->barrier);
	}

	scePthreadBarrierWait(&state->barrier);

	if (state->outputStream)
	{
		delete state->outputStream;
		state->outputStream = NULL;
	}

	return arg;
}

Song::Song()
	: _state(0)
{
}

Song::~Song()
{
	if (_state == NULL)
		return;

	Stop();

	// join and detach a thread for audio decode
	if (_state->threadDecode) {
		scePthreadJoin(_state->threadDecode, 0);
		_state->threadDecode = NULL;
	}

	// join and detach a thread for audio output
	if (_state->threadOutput) {
		scePthreadJoin(_state->threadOutput, 0);
		_state->threadOutput = NULL;
	}

	if (_state->barrier) {
		scePthreadBarrierDestroy(&_state->barrier);
		_state->barrier = NULL;
	}

	if (_state->eventFlag) {
		sceKernelDeleteEventFlag(_state->eventFlag);
		_state->eventFlag = NULL;
	}

	Allocator::Get()->release(_state->fileName);
	Allocator::Get()->release(_state);
}

bool Song::Load(const char* fileName)
{
	auto len = strlen(fileName) + 1;
	if (len > FILENAME_MAX || len == 0)
		return false;

	_state = (Media::SongState*)Allocator::Get()->allocate(sizeof(SongState));
	memset(_state, 0, sizeof(SongState));
	_state->fileName = (char*)Allocator::Get()->allocate(len);
	strcpy(_state->fileName, fileName);

	return true;
}

void Song::Play()
{
	if (_state == NULL)
		return;

	Stop();

	auto ret = 0;
	ScePthreadAttr attr = 0;
	SceKernelSchedParam schedParam;

	ret = sceKernelCreateEventFlag(&_state->eventFlag, "AudioBgm", SCE_KERNEL_EVF_ATTR_MULTI, 0, 0);
	if (ret < 0)
	{
		printf("error: sceKernelCreateEventFlag() failed: 0x%08X\n", ret);
		goto term;
	}

	ret = scePthreadBarrierInit(&_state->barrier, 0, 2, "AudioBgm");
	if (ret < 0)
	{
		printf("error: sceThreadBarrierInit() failed: 0x%08X\n", ret);
		goto term;
	}

	scePthreadAttrInit(&attr);
	scePthreadAttrSetstacksize(&attr, stackSize);
	scePthreadAttrSetinheritsched(&attr, SCE_PTHREAD_EXPLICIT_SCHED);
	schedParam.sched_priority = SCE_KERNEL_PRIO_FIFO_HIGHEST;
	scePthreadAttrSetschedparam(&attr, &schedParam);

	ret = scePthreadCreate(&_state->threadDecode, &attr, decodeMain, _state, "AudioDecBgm");
	if (ret < 0)
	{
		printf("error: scePthreadCreate() failed: 0x%08X\n", ret);
		goto term;
	}

	ret = scePthreadCreate(&_state->threadOutput, &attr, outputMain, _state, "AudioOutBgM");
	if (ret < 0)
	{
		printf("error: scePthreadCreate() failed: 0x%08X\n", ret);
		goto term;
	}

	_state->isRunning = true;
	sceKernelSetEventFlag(_state->eventFlag, EventFlags::Loaded);

term:
	if (attr)
	{
		scePthreadAttrDestroy(&attr);
		attr = 0;
	}
}

void Song::Resume()
{
	if (_state == NULL)
		return;

	_state->isPaused = false;
}

void Song::Pause()
{
	if (_state == NULL)
		return;

	_state->isPaused = true;
}

void Song::Stop()
{
	if (_state == NULL)
		return;

	// We're stopping playback explicitly, so we don't need a
	// notification that things have completed.
	_state->onSongFinished = NULL;
	_state->isRunning = false;
}

float Song::GetVolume()
{
	if (_state == NULL)
		return 1.0f;

	if (_state->outputStream == NULL)
		return 1.0f;

	return _state->outputStream->getOutput()->getVolume();
}

void Song::SetVolume(float value)
{
	if (_state == NULL)
		return;

	assert(value >= 0.0f || value <= 1.0f);

	if (_state->outputStream == NULL)
		return;

	_state->outputStream->getOutput()->setVolume(value);
}

float Song::GetPosition()
{
	if (_state == NULL || _state->decoder == NULL || _state->outputStream == NULL)
		return 0.0f;

	return _state->decoder->getProgress();
}

bool Song::GetIsRepeating()
{
	if (_state == NULL)
		return false;

	return _state->isLooping;
}

void Song::SetIsRepeating(bool value)
{
	if (_state == NULL)
		return;

	_state->isLooping = value;
}

void Song::RegisterFinishedHandler(SongFinishedHandler handler)
{
	if (_state == NULL)
		return;

	assert(_state->onSongFinished == NULL);
	_state->onSongFinished = handler;
}

