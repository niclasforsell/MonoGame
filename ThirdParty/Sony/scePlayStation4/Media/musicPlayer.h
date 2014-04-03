#pragma once

#include "predecls.h"

#include <stddef.h>
#include <stdint.h>

namespace Media {

class __declspec(dllexport) MusicPlayer
{
private:
	MusicPlayerState* _state;

public:
	MusicPlayer();
	~MusicPlayer();

	bool LoadAT9(const char* fileName);

	void Play();
	void Resume();
	void Pause();
	void Stop();

	float GetVolume();
	void SetVolume(float value);
};

} // namespace Media
