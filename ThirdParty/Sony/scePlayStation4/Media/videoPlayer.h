#pragma once

#include "predecls.h"

#include <ctime>
#include <scebase.h>
#include <sceavplayer.h>

namespace Graphics {
    class Texture;
}

namespace Media {

enum class VideoPlayerResult
{
	Ok = SCE_OK,

	ErrorInvalidParams = SCE_AVPLAYER_ERROR_INVALID_PARAMS,
	ErrorOperationFailed = SCE_AVPLAYER_ERROR_OPERATION_FAILED,
	ErrorNoMemory = SCE_AVPLAYER_ERROR_NO_MEMORY,
	ErrorNotSupported = SCE_AVPLAYER_ERROR_NOT_SUPPORTED
};

class CS_API VideoPlayer
{
	CS_IGNORE SceAvPlayerHandle _handle;
	CS_IGNORE int32_t _sourceID;

public:
	VideoPlayer();
	~VideoPlayer();

	Graphics::Texture* GetTexture();

	void Pause();
	void Resume();
	void Play(const char* filename);
	void Stop();
	void SetVolume(float volume);
	time_t GetPlayPosition();
};

} // namespace Media
