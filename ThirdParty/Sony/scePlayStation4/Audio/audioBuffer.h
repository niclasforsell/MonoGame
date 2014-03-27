#pragma once

#include "predecls.h"
#include <memory.h>

namespace Audio {

class __declspec(dllexport) AudioBuffer
{
	friend class SoundSystem;
	friend class SamplerVoice;

public:
	AudioBuffer(void *data, size_t dataSize);
	AudioBuffer(void *data, size_t dataSize, int numChannels, int blockAlign, bool isMSADPCM);
	~AudioBuffer(void);

protected:
	static void MSADPCM_to_PCM(void *data, size_t dataSize, int numChannels, int blockAlign, size_t *outDataSize, void **outData);

private:
	void *_waveformData;
	SceNgs2WaveformInfo *_waveformInfo;
	SceNgs2WaveformInfo *_loopedWaveformInfo;
};

} // namespace Audio
