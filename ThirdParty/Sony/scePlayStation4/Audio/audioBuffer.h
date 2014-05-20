#pragma once

#include "predecls.h"
#include <memory.h>

namespace Audio {

class CS_API AudioBuffer
{
	friend class SoundSystem;
	friend class SamplerVoice;

public:
	static AudioBuffer* FromPCM(void* data, size_t dataSize, int sampleRate, int numChannels);
	static AudioBuffer* FromRIFF(void* data, size_t dataSize);

	~AudioBuffer(void);

private:
	AudioBuffer() { }
	AudioBuffer(const AudioBuffer&);

	void *_waveformData;
	SceNgs2WaveformInfo *_waveformInfo;
	SceNgs2WaveformInfo *_loopedWaveformInfo;
};

} // namespace Audio
