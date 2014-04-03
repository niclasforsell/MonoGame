#include "musicPlayer.h"
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
	const SceKernelUseconds pauseSleepTime = 500000;

	enum EventFlags : uint64_t
	{
		Loaded      = 1 << 0,
		Stopping    = 1 << 1,
	};
}

// Private state hidden from public interface
struct Media::MusicPlayerState
{
	SceKernelEventFlag eventFlag;
	ScePthreadBarrier barrier;
	ScePthread threadDecode;
	ScePthread threadOutput;

	FileInputStream* inputStream;
	AudioOutputStream* outputStream;
	AudioDecoder* decoder;

	bool isPaused;
	bool isRunning;
};

void* decodeMain(void* arg)
{
	auto state = (Media::MusicPlayerState*)arg;
	sceKernelWaitEventFlag(state->eventFlag, EventFlags::Loaded, SCE_KERNEL_EVF_WAITMODE_AND, 0, 0);

	while (state->isRunning)
	{
		if (state->inputStream->isEmpty())
			break;

		if (state->isPaused)
			continue;

		auto ret = state->decoder->decode(state->inputStream, state->outputStream);
		if (ret < 0)
		{
			printf("ERROR: MusicPlayer decode failed: 0x%08X\n", ret);
			return arg;
		}
	}

	state->outputStream->end();

	scePthreadBarrierWait(&state->barrier);
	printf("MusicPlayer decode thread exiting.\n");
	return arg;
}

void* outputMain(void* arg)
{
	auto state = (Media::MusicPlayerState*)arg;
	sceKernelWaitEventFlag(state->eventFlag, EventFlags::Loaded, SCE_KERNEL_EVF_WAITMODE_AND, 0, 0);

	while(state->isRunning)
	{
		if (state->isPaused)
			continue;

		if (state->inputStream->isEmpty() && state->outputStream->isEmpty())
			break;

		auto ret = state->outputStream->output();
		if (ret < 0)
		{
			printf("ERROR: MusicPlayer output failed: 0x%08X\n", ret);
			return arg;
		}
	}

	scePthreadBarrierWait(&state->barrier);
	printf("MusicPlayer output thread exiting.\n");
	return arg;
}

MusicPlayer::MusicPlayer()
{
	_state = (Media::MusicPlayerState*)Allocator::Get()->allocate(sizeof(MusicPlayerState));
}

MusicPlayer::~MusicPlayer()
{
	Unload();
	Allocator::Get()->release(_state);
}

bool MusicPlayer::LoadAAC(const char* fileName)
{
	assert(fileName != NULL);
	Unload();

	const uint32_t fileSize = File(fileName, "rb").size();
	_state->inputStream = new FileInputStream(fileSize);
	assert(_state->inputStream);

	auto ret = _state->inputStream->open(fileName, "rb");
	assert(ret >= 0);

	ret = _state->inputStream->input();
	_state->inputStream->end();

	_state->decoder = new AudioDecoderM4aac(_state->inputStream);
	assert(_state->decoder);

	_state->outputStream = new AudioOutputStream(outputStreamSize, sizeof(int16_t) * AUDIO_STEREO * outputStreamGrain);
	assert(_state->outputStream);

	ret = _state->outputStream->open(outputStreamGrain, _state->decoder->sampleRate(), AudioOutputSystem::param(_state->decoder->numChannels()));
	assert(ret >= 0);

	return true;
}

void MusicPlayer::Unload()
{
	Stop();

	if (_state->outputStream)
	{
		Allocator::Get()->release(_state->outputStream);
		_state->outputStream = NULL;
	}

	if (_state->decoder)
	{
		Allocator::Get()->release(_state->decoder);
		_state->decoder = NULL;
	}

	if (_state->inputStream)
	{
		Allocator::Get()->release(_state->inputStream);
		_state->inputStream = NULL;
	}
}

void MusicPlayer::Play()
{
	if (_state->isRunning)
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

void MusicPlayer::Resume()
{
	_state->isPaused = false;
}

void MusicPlayer::Pause()
{
	_state->isPaused = true;
}

void MusicPlayer::Stop()
{
	if (!_state->isRunning)
		return;

	sceKernelClearEventFlag(_state->eventFlag, EventFlags::Loaded);
	_state->isRunning = false;

	// join and detach a thread for audio decode
	if (_state->threadDecode) {
		scePthreadJoin(_state->threadDecode, 0);
		_state->threadDecode = 0;
	}

	// join and detach a thread for audio output
	if (_state->threadOutput) {
		scePthreadJoin(_state->threadOutput, 0);
		_state->threadOutput = 0;
	}

	if (_state->barrier) {
		scePthreadBarrierDestroy(&_state->barrier);
		_state->barrier = 0;
	}

	if (_state->eventFlag) {
		sceKernelDeleteEventFlag(_state->eventFlag);
		_state->eventFlag = 0;
	}
}

float MusicPlayer::GetVolume()
{
	if (_state->outputStream == NULL)
		return 1.0f;

	return _state->outputStream->getOutput()->getVolume();
}

void MusicPlayer::SetVolume(float value)
{
	if (_state->outputStream == NULL)
		return;

	_state->outputStream->getOutput()->setVolume(value);
}
