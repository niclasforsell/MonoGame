#pragma once

#include "predecls.h"
#include <memory.h>

namespace Audio {

class CS_API AudioBuffer
{
	friend class SoundSystem;
	friend class SamplerVoice;

public:

	AudioBuffer();
	virtual ~AudioBuffer();

	void InitPCM(void* data, size_t dataSize, int sampleRate, int numChannels);
	void InitRIFF(void* data, size_t dataSize);

private:
	AudioBuffer(const AudioBuffer&);

	void *_waveformData;
	SceNgs2WaveformInfo *_waveformInfo;
	SceNgs2WaveformInfo *_loopedWaveformInfo;
};

} // namespace Audio
