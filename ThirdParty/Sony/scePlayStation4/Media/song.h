#pragma once

#include "predecls.h"

#include <stddef.h>
#include <stdint.h>

namespace Media {

class __declspec(dllexport) Song
{
private:
	SongState* _state;

public:
	Song();
	~Song();

	bool Load(const char* fileName);

	void Play();
	void Resume();
	void Pause();
	void Stop();

	float GetVolume();
	void SetVolume(float value);

	float GetPosition();

	bool GetIsRepeating();
	void SetIsRepeating(bool value);
};

} // namespace Media
