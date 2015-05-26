#include "audioBuffer.h"

#include "../allocator.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ngs2.h>

using namespace Audio;


AudioBuffer::AudioBuffer()
{
	_waveformData = nullptr;
	_waveformInfo = nullptr;
	_loopedWaveformInfo = nullptr;
}

void AudioBuffer::InitPCM(void* data, size_t dataSize, int sampleRate, int numChannels)
{
	assert(dataSize > 0);
	assert(numChannels > 0);

	_waveformInfo = new SceNgs2WaveformInfo();
	_waveformData = mem::alloc(dataSize);
	memcpy(_waveformData, data, dataSize);

	int numSamples = (dataSize / numChannels) / 2;

	memset(_waveformInfo, 0, sizeof(SceNgs2WaveformInfo));
	_waveformInfo->format.waveformType = SCE_NGS2_WAVEFORM_TYPE_PCM_I16L;
	_waveformInfo->format.numChannels = numChannels;
	_waveformInfo->format.sampleRate = sampleRate;
	_waveformInfo->dataSize = dataSize;
	_waveformInfo->numSamples = numSamples;
	_waveformInfo->audioUnitSize = 2 * numChannels;
	_waveformInfo->numAudioUnitSamples = 1;
	_waveformInfo->numAudioUnitPerFrame = 1;
	_waveformInfo->audioFrameSize = 2 * numChannels;
	_waveformInfo->numAudioFrameSamples = 1;
	_waveformInfo->numDelaySamples = 0;
	_waveformInfo->numBlocks = 1;
	_waveformInfo->aBlock[0].dataOffset = 0;
	_waveformInfo->aBlock[0].dataSize = dataSize;
	_waveformInfo->aBlock[0].numRepeats = 0;
	_waveformInfo->aBlock[0].numSkipSamples = 0;
	_waveformInfo->aBlock[0].numSamples = numSamples;
	_waveformInfo->aBlock[0].reserved = 0;
	_waveformInfo->aBlock[0].userData = 0;

	_loopedWaveformInfo = mem::alloc<SceNgs2WaveformInfo>();
	memcpy(_loopedWaveformInfo, _waveformInfo, sizeof(SceNgs2WaveformInfo));
	_loopedWaveformInfo->aBlock[0].numRepeats = SCE_NGS2_WAVEFORM_BLOCK_REPEAT_INFINITE;
}

void AudioBuffer::InitRIFF(void* data, size_t dataSize)
{
	_waveformData = mem::alloc(dataSize);
	memcpy(_waveformData, data, dataSize);

	_waveformInfo = new SceNgs2WaveformInfo();
	_loopedWaveformInfo = mem::alloc<SceNgs2WaveformInfo>();

	int errorCode;

	errorCode = sceNgs2ParseWaveformData(_waveformData, dataSize, _waveformInfo);
	assert(errorCode >= 0);

	memcpy(_loopedWaveformInfo, _waveformInfo, sizeof(SceNgs2WaveformInfo));

	auto x = 0;
	if (_loopedWaveformInfo->loopBeginPosition != 0)
		x++;

	for(; x < _loopedWaveformInfo->numBlocks; x++)
		_loopedWaveformInfo->aBlock[x].numRepeats = SCE_NGS2_WAVEFORM_BLOCK_REPEAT_INFINITE;
}

AudioBuffer::~AudioBuffer()
{
	if (_waveformData)
	{
		mem::free(_waveformData);
		_waveformData = NULL;
	}

	if (_waveformInfo)
	{
		delete _waveformInfo;
		_waveformInfo = NULL;
	}

	if (_loopedWaveformInfo)
	{
		mem::free(_loopedWaveformInfo);
		_loopedWaveformInfo = NULL;
	}
}
