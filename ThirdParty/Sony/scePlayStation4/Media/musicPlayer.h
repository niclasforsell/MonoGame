#pragma once

#include "predecls.h"
#include <stddef.h>
#include <stdint.h>

namespace Media {

class __declspec(dllexport) MusicPlayer
{
private:
	static bool _isInitialized;

protected:
	uint8_t* _srcBuffer;
	size_t _srcBufferSize;

	uint8_t* _outBuffer;
	size_t _outBufferSize;
	size_t _outBufferUsed;

	int32_t _decoder;
	int32_t _output;

	uint32_t _numChannels;
	Audio::AudioOut* _audioOut;

	virtual void Unload();

public:
	MusicPlayer();
	virtual ~MusicPlayer();

	virtual bool Load(const char* fileName);

	virtual void Play();
	virtual void Resume();
	virtual void Pause();
	virtual void Stop();

	float GetVolume();
	void SetVolume(float value);
};

} // namespace Media
