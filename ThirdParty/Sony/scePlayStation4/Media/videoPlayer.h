#pragma once

#include "predecls.h"

#include <ctime>
#include <scebase.h>
#include <sceavplayer.h>
#include <sceavplayer_ex.h>
#include <pthread.h>

namespace std {
	class mutex;
	class thread;
	class condition_variable;
}

namespace Graphics {
	class GraphicsSystem;
	class RenderTarget;
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
public:
	CS_IGNORE SceAvPlayerHandle _handle;
	CS_IGNORE int32_t _sourceID;
	CS_IGNORE int32_t _videoOutHandle;
	CS_IGNORE std::thread* _videoThread;
	CS_IGNORE std::thread* _audioThread;

	CS_IGNORE std::mutex* _frameMutex;
	CS_IGNORE bool _frameAvailable;
	CS_IGNORE SceAvPlayerFrameInfoEx _videoFrame;
	CS_IGNORE SceAvPlayerFrameInfoEx _audioFrame;

	CS_IGNORE Graphics::GraphicsSystem* _graphics;
	CS_IGNORE Graphics::RenderTarget* _renderTarget;

public:
	VideoPlayer(Graphics::GraphicsSystem* graphics, Graphics::RenderTarget* renderTarget);
	~VideoPlayer();

	void GrabFrame();

	void Pause();
	void Resume();
	void Play(const char* filename);
	void Stop();
	void SetVolume(float volume);
	time_t GetPlayPosition();
};

} // namespace Media
