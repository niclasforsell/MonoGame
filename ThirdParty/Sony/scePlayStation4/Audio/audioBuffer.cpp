#include "audioBuffer.h"

#include "../allocator.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ngs2.h>

using namespace Audio;

AudioBuffer* AudioBuffer::FromPCM(void* data, size_t dataSize, int sampleRate, int numChannels)
{
	assert(dataSize > 0);
	assert(numChannels > 0);

	auto result = new AudioBuffer();

	result->_waveformInfo = (SceNgs2WaveformInfo*)Allocator::Get()->allocate(sizeof(SceNgs2WaveformInfo));
	result->_waveformData = Allocator::Get()->allocate(dataSize);
	memcpy(result->_waveformData, data, dataSize);

	int numSamples = (dataSize / numChannels) / 2;

	memset(result->_waveformInfo, 0, sizeof(SceNgs2WaveformInfo));
	result->_waveformInfo->format.waveformType = SCE_NGS2_WAVEFORM_TYPE_PCM_I16L;
	result->_waveformInfo->format.numChannels = numChannels;
	result->_waveformInfo->format.sampleRate = sampleRate;
	result->_waveformInfo->dataSize = dataSize;
	result->_waveformInfo->numSamples = numSamples;
	result->_waveformInfo->audioUnitSize = 2 * numChannels;
	result->_waveformInfo->numAudioUnitSamples = 1;
	result->_waveformInfo->numAudioUnitPerFrame = 1;
	result->_waveformInfo->audioFrameSize = 2 * numChannels;
	result->_waveformInfo->numAudioFrameSamples = 1;
	result->_waveformInfo->numDelaySamples = 0;
	result->_waveformInfo->numBlocks = 1;
	result->_waveformInfo->aBlock[0].dataOffset = 0;
	result->_waveformInfo->aBlock[0].dataSize = dataSize;
	result->_waveformInfo->aBlock[0].numRepeats = 0;
	result->_waveformInfo->aBlock[0].numSkipSamples = 0;
	result->_waveformInfo->aBlock[0].numSamples = numSamples;
	result->_waveformInfo->aBlock[0].reserved = 0;
	result->_waveformInfo->aBlock[0].userData = 0;

	result->_loopedWaveformInfo = (SceNgs2WaveformInfo*)Allocator::Get()->allocate(sizeof(SceNgs2WaveformInfo));
	memcpy(result->_loopedWaveformInfo, result->_waveformInfo, sizeof(SceNgs2WaveformInfo));
	result->_loopedWaveformInfo->aBlock[0].numRepeats = SCE_NGS2_WAVEFORM_BLOCK_REPEAT_INFINITE;

	return result;
}

AudioBuffer* AudioBuffer::FromRIFF(void* data, size_t dataSize)
{
	auto result = new AudioBuffer();

	result->_waveformData = Allocator::Get()->allocate(dataSize);
	memcpy(result->_waveformData, data, dataSize);

	result->_waveformInfo = (SceNgs2WaveformInfo*)Allocator::Get()->allocate(sizeof(SceNgs2WaveformInfo));
	result->_loopedWaveformInfo = (SceNgs2WaveformInfo*)Allocator::Get()->allocate(sizeof(SceNgs2WaveformInfo));

	int errorCode;

	errorCode = sceNgs2ParseWaveformData(result->_waveformData, dataSize, result->_waveformInfo);
	assert(errorCode >= 0);

	memcpy(result->_loopedWaveformInfo, result->_waveformInfo, sizeof(SceNgs2WaveformInfo));

	auto x = 0;
	if (result->_loopedWaveformInfo->loopBeginPosition != 0)
		x++;

	for(; x < result->_loopedWaveformInfo->numBlocks; x++)
		result->_loopedWaveformInfo->aBlock[x].numRepeats = SCE_NGS2_WAVEFORM_BLOCK_REPEAT_INFINITE;

	return result;
}

AudioBuffer::~AudioBuffer(void)
{
	if (_waveformData)
	{
		Allocator::Get()->release(_waveformData);
		_waveformData = NULL;
	}

	if (_waveformInfo)
	{
		Allocator::Get()->release(_waveformInfo);
		_waveformInfo = NULL;
	}

	if (_loopedWaveformInfo)
	{
		Allocator::Get()->release(_loopedWaveformInfo);
		_loopedWaveformInfo = NULL;
	}
}
