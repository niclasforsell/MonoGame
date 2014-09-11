#pragma once

#include "predecls.h"
#include <ngs2.h>

namespace Audio {

class AudioBuffer;

// This enum mirrors the one in XNA
enum class SoundState
{
    Playing,
    Paused,
    Stopped
};

class CS_API SamplerVoice
{
	friend class SoundSystem;

protected:

	SamplerVoice(unsigned int id, SceNgs2Handle rackHandle, SceNgs2Handle voiceHandle, AudioBuffer *buffer);
	~SamplerVoice(void);

	SceNgs2Handle _rackHandle;

	SceNgs2Handle _voiceHandle;
	unsigned int _voiceHandleID;
	float _pitch;
	float _pan;
	float _volume;
	bool _looped;
	AudioBuffer *_buffer;


	void GetPanLevels(float angle, float *panLevels);
	void SetMatrixLevels(float *panLevels);

public:

	void SetFade(float fadeinTime, float fadeOutTime);

	SoundState GetState();

	float GetVolume();
	void SetVolume(float newVol);
	
	bool GetLooped();
	void SetLooped(bool loop);

	void SetPan(float pan);
	float GetPan();

	void SetPitch(float pitch);
	float GetPitch();

};

} // namespace Audio
