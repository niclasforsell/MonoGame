#pragma once

#include "predecls.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <ctime>
#include <scebase.h>
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

class CS_API VideoPlayer
{
public:
	CS_IGNORE SceAvPlayerHandle _handle;
	CS_IGNORE int32_t _sourceID;
	CS_IGNORE int32_t _videoOutHandle;

	CS_IGNORE std::thread* _videoThread;
	CS_IGNORE std::thread* _audioThread;

	CS_IGNORE bool _frameAvailable;
	CS_IGNORE std::mutex _frameLock;
	CS_IGNORE std::condition_variable _decodeReady;
	CS_IGNORE std::condition_variable _displayReady;

	CS_IGNORE SceAvPlayerFrameInfoEx _videoFrame;
	CS_IGNORE SceAvPlayerFrameInfoEx _audioFrame;

	CS_IGNORE Graphics::GraphicsSystem* _graphics;

public:
	VideoPlayer(Graphics::GraphicsSystem* graphics);
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
