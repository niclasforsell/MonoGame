#pragma once

#include "predecls.h"

#include <ctime>
#include <scebase.h>
#include <kernel.h>
#include <sceavplayer.h>
#include <sceavplayer_ex.h>

namespace Graphics {
	class GraphicsSystem;
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

enum class VideoPlayerState
{
	Not_Loaded,
	Stopped,
	Paused,
	Playing,
};

class CS_API VideoPlayer
{
public:
	CS_IGNORE SceAvPlayerHandle _handle;
	CS_IGNORE int32_t _sourceID;

	CS_IGNORE ScePthread _videoThread;
	CS_IGNORE ScePthread _audioThread;

	CS_IGNORE ScePthreadMutex _frameMutex;
	CS_IGNORE SceAvPlayerFrameInfoEx _videoFrame;

	CS_IGNORE ScePthreadMutex _audioMutex;
	CS_IGNORE float _volume;

	CS_IGNORE Graphics::GraphicsSystem* _graphics;

	CS_IGNORE VideoPlayerState _state;

public:
	VideoPlayer(Graphics::GraphicsSystem* graphics);
	~VideoPlayer();

	bool GrabFrame();

	void Pause();
	void Resume();
	void Play(const char* filename);
	void Stop();
	void SetVolume(float volume);
	time_t GetPlayPosition();
};

} // namespace Media
