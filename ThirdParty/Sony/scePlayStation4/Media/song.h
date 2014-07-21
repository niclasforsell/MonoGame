#pragma once

#include "predecls.h"

#include <stddef.h>
#include <stdint.h>

namespace Media {

typedef void(*SongFinishedHandler)(void);

class CS_API Song
{
private:
	SongState* _state;

public:
	Song();
	~Song();

	bool Load(const char* fileName);

	void Play();
	void PlayFrom(float seconds, float fadeInSeconds);
	void Resume();
	void Pause();
	void Stop();

	float GetVolume();
	void SetVolume(float value);

	float GetPosition();

	void RegisterFinishedHandler(SongFinishedHandler handler);
};

} // namespace Media
