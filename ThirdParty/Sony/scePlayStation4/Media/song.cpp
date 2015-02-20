#include "song.h"
#include "../allocator.h"
#include "stream.h"
#include "audio_decoder.h"
#include "at9/audio_decoder_at9.h"
#include "m4aac/audio_decoder_m4aac.h"
#include "mp3/audio_decoder_mp3.h"

#include <assert.h>
#include <atomic>
#include <libsysmodule.h>
#include <audiodec.h>
#include <audioout.h>
#include <sceerror.h>
#include <string.h>

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

		AllDone = Decoder | Output,
		Everything = Loaded | Decoder | Output
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

	std::atomic<float> startSeconds;
	std::atomic<float> fadeInSeconds;
	std::atomic<float> volume;
	std::atomic<float> position;

	std::atomic<bool> isPaused;
	std::atomic<bool> isRunning;
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
	assert(state->fileName != NULL);

	const char* fileExt = strrchr(state->fileName, '.');

	const uint32_t inputStreamSize = File(state->fileName, "rb").size();
	assert(inputStreamSize > 0);
	if (inputStreamSize < 0)
	{
		printf("ERROR: Failed to open song file '%s'\n", state->fileName);
		goto term;
	}

	state->inputStream = new FileInputStream(inputStreamSize);
	assert(state->inputStream);
	ret = state->inputStream->open(state->fileName, "rb");
	if (ret < 0)
	{
		printf("ERROR: Failed to open input stream for song '%s'\n", state->fileName);
		goto term;
	}
	assert(ret >= 0);

	ret = state->inputStream->input();
	assert(ret >= 0);
	state->inputStream->end();

	if (strncmp(fileExt, ".mp4", 4) == 0)
	{
		state->decoder = new AudioDecoderM4aac(state->inputStream);
	}
	else if (strncmp(fileExt, ".at9", 4) == 0)
	{
		state->decoder = new AudioDecoderAt9(state->inputStream);
	}
	else if (strncmp(fileExt, ".mp3", 4) == 0)
	{
		state->decoder = new AudioDecoderMp3(state->inputStream);
	}
	else
	{
		printf("ERROR: Unknown song format: %s\n", fileExt);
		goto term;
	}
	assert(state->decoder);
	if (state->decoder == NULL)
	{
		printf("ERROR: Couldn't create decoder for %s\n", state->fileName);
		goto term;
	}

	// Signal we're loaded and wait for the sync point.
	sceKernelSetEventFlag(state->eventFlag, EventFlags::Decoder);
	scePthreadBarrierWait(&state->barrier);

	// Seek ahead to start position if specified
	if (state->startSeconds > 0.0f)
		state->decoder->seek(state->inputStream, state->startSeconds);

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
			goto term;
		}

		state->position = state->decoder->getElapsedSeconds();
	}

	printf("Exiting decode loop.\n");
	state->isRunning = false;
	state->inputStream->end();
	state->outputStream->end();

term:
	sceKernelClearEventFlag(state->eventFlag, EventFlags::Everything);
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
	}

	printf("Decode thread exiting.\n");
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

	auto fadeEnabled = state->fadeInSeconds > 0.0f;
	auto audioOut = state->outputStream->getOutput();	
	auto startFade = state->startSeconds.load();
	auto finishFade = state->startSeconds + state->fadeInSeconds;
	
	float stateVolume = state->volume;
	if (fadeEnabled)
		audioOut->setVolume(0.0f);
	else
		audioOut->setVolume(stateVolume);

	while(state->isRunning)
	{
		if (state->isPaused)
			continue;

		if (state->inputStream->isEmpty() && state->outputStream->isEmpty())
			break;
		
		stateVolume = state->volume;

		if (fadeEnabled)
		{
			auto pos = state->decoder->getElapsedSeconds();
			if (pos >= startFade && pos <= finishFade)
			{
				auto amount = (pos - startFade) / state->fadeInSeconds;
				audioOut->setVolume(amount * stateVolume);
			}

			if (pos > finishFade)
			{
				fadeEnabled = false;
				audioOut->setVolume(stateVolume);
			}
		}
		else
		{
			// Note that the value of state->volume can be changed at any time
			// by the user. We must set this even it even if no fade is occuring.
			audioOut->setVolume(stateVolume);
		}
		
		auto ret = state->outputStream->output();
		if (ret < 0)
		{
			printf("ERROR: MusicPlayer output failed: 0x%08X\n", ret);
			goto term;
		}
	}

	printf("Exiting output loop.\n");
	state->isRunning = false;
	state->inputStream->end();
	state->outputStream->end();

term:
	sceKernelSetEventFlag(state->eventFlag, EventFlags::Output);
	scePthreadBarrierWait(&state->barrier);

	if (state->outputStream)
	{
		delete state->outputStream;
		state->outputStream = NULL;
	}

	printf("Output thread exiting.\n");
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

	// join and detach a thread for audio output
	printf("Joining output thread.\n");
	if (_state->threadOutput) {
		scePthreadJoin(_state->threadOutput, 0);
		_state->threadOutput = NULL;
	}

	// join and detach a thread for audio decode
	printf("Joining decode thread.\n");
	if (_state->threadDecode) {
		scePthreadJoin(_state->threadDecode, 0);
		_state->threadDecode = NULL;
	}

	if (_state->barrier) {
		scePthreadBarrierDestroy(&_state->barrier);
		_state->barrier = NULL;
	}

	if (_state->eventFlag) {
		sceKernelDeleteEventFlag(_state->eventFlag);
		_state->eventFlag = NULL;
	}

	mem::free(_state->fileName);
	mem::free(_state);

	printf("Song cleanup complete.\n");
}

bool Song::Load(const char* fileName)
{
	auto len = strlen(fileName) + 1;
	if (len > FILENAME_MAX || len == 0)
		return false;

	_state = mem::alloc<Media::SongState>();
	memset(_state, 0, sizeof(SongState));
	_state->fileName = mem::alloc<char>(len);
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
	scePthreadAttrSetaffinity(&attr, (1 << 4) | (1 << 5));

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

void Song::PlayFrom(float seconds, float fadeInSeconds)
{
	_state->startSeconds = seconds;
	_state->fadeInSeconds = fadeInSeconds;
	Play();
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
	assert(_state != NULL);			
	
	return _state->volume;	
}

void Song::SetVolume(float value)
{
	assert(value >= 0.0f || value <= 1.0f);
	assert(_state != NULL);
	
	_state->volume = value;	
}

float Song::GetPosition()
{	
	if (_state == NULL)
		return 0.0f;

	return _state->position;
}

void Song::RegisterFinishedHandler(SongFinishedHandler handler)
{
	if (_state == NULL)
		return;

	// JCF: This is not thread safe!
	assert(_state->onSongFinished == NULL);
	_state->onSongFinished = handler;
}

