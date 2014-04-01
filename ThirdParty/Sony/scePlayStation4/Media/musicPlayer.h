#pragma once

#include "predecls.h"

namespace Media {

class __declspec(dllexport) MusicPlayer
{
public:
	static void Initialize();
	static void Terminate();

	static void Play(void* buffer, size_t size, int sampleRate, int numChannels);
	static void Stop();
};

} // namespace Media
