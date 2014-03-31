#include "audioBuffer.h"

#include "../allocator.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ngs2.h>

using namespace Audio;

AudioBuffer::AudioBuffer(void *data, size_t dataSize)
{
	_waveformData = Allocator::Get()->allocate(dataSize);
	memcpy(_waveformData, data, dataSize);

	_waveformInfo = (SceNgs2WaveformInfo*)Allocator::Get()->allocate(sizeof(SceNgs2WaveformInfo));
	_loopedWaveformInfo = (SceNgs2WaveformInfo*)Allocator::Get()->allocate(sizeof(SceNgs2WaveformInfo));

	int errorCode;

	errorCode = sceNgs2ParseWaveformData(_waveformData, dataSize, _waveformInfo);
	assert(errorCode >= 0);

	memcpy(_loopedWaveformInfo, _waveformInfo, sizeof(SceNgs2WaveformInfo));

	for(auto x = 0; x < _loopedWaveformInfo->numBlocks; x++)
		_loopedWaveformInfo->aBlock[x].numRepeats = SCE_NGS2_WAVEFORM_BLOCK_REPEAT_INFINITE;	
}

AudioBuffer::AudioBuffer(void *data, size_t dataSize, int numChannels, int blockAlign, bool isMSADPCM)
{
	_waveformInfo = (SceNgs2WaveformInfo*)Allocator::Get()->allocate(sizeof(SceNgs2WaveformInfo));
	_loopedWaveformInfo = (SceNgs2WaveformInfo*)Allocator::Get()->allocate(sizeof(SceNgs2WaveformInfo));

	if (isMSADPCM)
		MSADPCM_to_PCM(data, dataSize, numChannels, blockAlign, &dataSize, &_waveformData);
	else
	{
		_waveformData = Allocator::Get()->allocate(dataSize);
		memcpy(_waveformData, data, dataSize);
	}

	int numSamples = (dataSize / numChannels) / 2;

	memset(_waveformInfo, 0, sizeof(SceNgs2WaveformInfo));
	_waveformInfo->format.waveformType = SCE_NGS2_WAVEFORM_TYPE_PCM_I16L;
	_waveformInfo->format.numChannels = numChannels;
	_waveformInfo->format.sampleRate = 44100;
	_waveformInfo->format.configData = 0;
	_waveformInfo->format.frameOffset = 0;
	_waveformInfo->format.frameMargin = 0;
	_waveformInfo->dataOffset = 0;
	_waveformInfo->dataSize = dataSize;
	_waveformInfo->loopBeginPosition = 0;
	_waveformInfo->loopEndPosition = 0;
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

	memcpy(_loopedWaveformInfo, _waveformInfo, sizeof(SceNgs2WaveformInfo));
	_loopedWaveformInfo->aBlock[0].numRepeats = SCE_NGS2_WAVEFORM_BLOCK_REPEAT_INFINITE;
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

static const int AdaptionTable[] = {
	230, 230, 230, 230, 307, 409, 512, 614,
	768, 614, 512, 409, 307, 230, 230, 230
};
static const int AdaptCoeff_1[] = {
	256, 512, 0, 192, 240, 460, 392
};
static const int AdaptCoeff_2[] = {
	0, -256, 0, 64, 0, -208, -232
};

static short calculateSample(
	uint8_t nibble,
	uint8_t predictor,
	short &sample_1,
	short &sample_2,
	short &delta)
{
	// Get a signed number out of the nibble. We need to retain the
	// original nibble value for when we access AdaptionTable[].
	int8_t signedNibble = (int8_t) nibble;
	if ((signedNibble & 0x8) == 0x8) {
		signedNibble -= 0x10;
	}

	// Calculate new sample
	int sampleInt = (
		(	(sample_1 * AdaptCoeff_1[predictor]) +
		(sample_2 * AdaptCoeff_2[predictor])
		) /256
		);
	sampleInt += signedNibble * delta;

	// Clamp result to 16-bit
	short sample;
	if (sampleInt < __SHRT_MIN) {
		sample = __SHRT_MIN;
	} else if (sampleInt > __SHRT_MAX) {
		sample = __SHRT_MAX;
	} else {
		sample = (short) sampleInt;
	}

	// Shuffle samples, get new delta
	sample_2 = sample_1;
	sample_1 = sample;
	delta = (short) (AdaptionTable[nibble] * delta / 256);

	// Saturate the delta to a lower bound of 16
	if (delta < 16)
		delta = 16;

	return sample;
}

void AudioBuffer::MSADPCM_to_PCM(void *data, size_t dataSize, int numChannels, int blockAlign, size_t *outDataSize, void **outData)
{
	// Assuming the whole stream is what we want.
	auto fileLength = dataSize - blockAlign;

	assert(numChannels == 2);

	auto samples = ((blockAlign + 15) * 2);
	auto fileBlocks = dataSize / (14 + samples);

	//System.Console.WriteLine("blockAlign = {0}", blockAlign);
	//System.Console.WriteLine("samples = {0}", samples);
	//System.Console.WriteLine("fileLength = {0}", fileLength);
	//System.Console.WriteLine("Source.BaseStream.Length = {0}", Source.BaseStream.Length);

	*outDataSize = ((samples * 4) + 8) * fileBlocks;
	*outData = Allocator::Get()->allocate(*outDataSize);

	uint8_t *source = (uint8_t*)data;
	uint8_t *sourceEnd = ((uint8_t*)data) + dataSize;
	short *pcmOut = (short*)*outData;

	// Read to the end of the file.
	auto blocks = 0;
	while (source != sourceEnd) 
	{
		blocks++;
		assert(source < sourceEnd);

		// Read block preamble
		uint8_t l_predictor = source[0];
		uint8_t r_predictor = source[1];
		short l_delta = *(short*)(source+2);
		short r_delta = *(short*)(source+4);
		short l_sample_1 = *(short*)(source+6);
		short r_sample_1 = *(short*)(source+8);
		short l_sample_2 = *(short*)(source+10);
		short r_sample_2 = *(short*)(source+12);
		source += 14;

		//System.Console.WriteLine("pcmData.Length = {0}", pcmData.Length);
		//System.Console.WriteLine("output.Position = {0}", output.Position);

		// Send the initial samples straight to PCM out.
		pcmOut[0] = l_sample_2;
		pcmOut[1] = r_sample_2;
		pcmOut[2] = l_sample_1;
		pcmOut[3] = r_sample_1;
		pcmOut += 4;

		// Go through the bytes in this MSADPCM block.
		for (auto i = 0; i < samples; i++) 
		{
			// Each sample is one half of a nibbleBlock.
			auto nibbleBlock = source[0];
			source++;

			// Left channel...
			pcmOut[0] =
				calculateSample(
				(uint8_t)(nibbleBlock >> 4), // Upper half
				l_predictor,
				l_sample_1,
				l_sample_2,
				l_delta
				);

			// Right channel...
			pcmOut[1] =
				calculateSample(
				(uint8_t)(nibbleBlock & 0xF), // Lower half
				r_predictor,
				r_sample_1,
				r_sample_2,
				r_delta
				);

			pcmOut += 2;
			//System.Console.WriteLine("output.Position = {0}", output.Position);
		}
	}
}