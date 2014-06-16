#include "videoPlayer.h"
#include "audio_output.h"
#include "../allocator.h"
#include "../Graphics/graphicsSystem.h"

#include <assert.h>
#include <memory>
#include <libsysmodule.h>
#include <sceerror.h>
#include <gnm.h>
#include <sceavplayer_ex.h>
#include <video_out.h>
#include <audioout.h>

using namespace sce;
using namespace Graphics;
using namespace Media;

namespace {
	void* videoAlloc(void* argP, uint32_t argAlignment, uint32_t argSize) { return mem::alloc(argSize, argAlignment); }
	void videoFree(void* argP, void* argMemory) { mem::free(argP); }
	void* videoAllocTexture(void* argP, uint32_t argAlignment, uint32_t argSize) { return mem::alloc(argSize, argAlignment); }
	void videoFreeTexture(void* argP, void* argMemory) { mem::free(argP); }

	void* videoOutputThread(void* arg)
	{
		auto player = (VideoPlayer*)arg;
		assert(player != nullptr);

		SceAvPlayerFrameInfoEx videoFrame;
		memset(&videoFrame, 0, sizeof(SceAvPlayerFrameInfoEx));

		while (sceAvPlayerIsActive(player->_handle))
		{
			if(sceAvPlayerGetVideoDataEx(player->_handle, &videoFrame))
			{
				scePthreadMutexLock(&player->_frameMutex);
				memcpy(&player->_videoFrame, &videoFrame, sizeof(SceAvPlayerFrameInfoEx));
				scePthreadMutexUnlock(&player->_frameMutex);
			}
		}

		scePthreadExit(nullptr);
		return nullptr;
	}

	void* audioOutputThread(void* arg)
	{
		auto player = (VideoPlayer*)arg;
		assert(player != nullptr);

		const uint32_t outputStreamSize = 128 * 1024;
		const uint32_t outputStreamGrain = 1024;
		AudioOutput output;
		output.open(outputStreamGrain, 48000, SCE_AUDIO_OUT_PARAM_FORMAT_S16_STEREO);

		void* silence = mem::alloc(4096, 0x20);
		memset(silence, 0, 4096);

		SceAvPlayerFrameInfo audioFrame;
		memset(&audioFrame, 0, sizeof(SceAvPlayerFrameInfo));

		while (sceAvPlayerIsActive(player->_handle))
		{
			scePthreadMutexLock(&player->_audioMutex);
			auto volume = player->_volume;
			auto isMuted = player->_isMuted || volume < 0.001f;
			scePthreadMutexUnlock(&player->_audioMutex);

			if (output.getVolume() != volume)
				output.setVolume(volume);

			if (sceAvPlayerGetAudioData(player->_handle, &audioFrame) && !isMuted)
				output.output(audioFrame.pData);
			else
				output.output(silence);


		}

		output.close();
		mem::free(silence);

		scePthreadExit(nullptr);
		return nullptr;
	}
}

VideoPlayer::VideoPlayer(GraphicsSystem* graphics)
{
	if (sceSysmoduleIsLoaded(SCE_SYSMODULE_AV_PLAYER) != SCE_SYSMODULE_LOADED)
	{
		auto ret = sceSysmoduleLoadModule(SCE_SYSMODULE_AV_PLAYER);
		assert(ret == SCE_OK);
	}

	memset(&_videoFrame, 0, sizeof(SceAvPlayerFrameInfoEx));
	scePthreadMutexInit(&_frameMutex, NULL, "av_frame_mutex");

	SceAvPlayerInitData param;
	memset(&param, 0, sizeof(SceAvPlayerInitData));
	param.memoryReplacement.objectPointer = NULL;
	param.memoryReplacement.allocate = videoAlloc;
	param.memoryReplacement.deallocate = videoFree;
	param.memoryReplacement.allocateTexture = videoAllocTexture;
	param.memoryReplacement.deallocateTexture = videoFreeTexture;

	param.debugLevel = SCE_AVPLAYER_DBG_INFO;
	param.basePriority = 160;
	param.numOutputVideoFrameBuffers = 6;
	param.autoStart = true;
	param.defaultLanguage = "eng";

	_state = VideoPlayerState::Not_Loaded;
	_handle = sceAvPlayerInit(&param);
	_graphics = graphics;
	_sourceID = -1;
	_volume = 1.0f;
	_isMuted = false;
	_isLooped = false;
}

VideoPlayer::~VideoPlayer()
{
	sceAvPlayerStop(_handle);
	scePthreadJoin(_audioThread, NULL);
	scePthreadJoin(_videoThread, NULL);

	scePthreadMutexLock(&_frameMutex);
	memset(&_videoFrame, 0, sizeof(SceAvPlayerFrameInfoEx));
	scePthreadMutexUnlock(&_frameMutex);

	sceAvPlayerClose(_handle);
	_handle = nullptr;
	_sourceID = -1;
	_state == VideoPlayerState::Not_Loaded;
}

bool VideoPlayer::GrabFrame()
{
	if (scePthreadMutexTrylock(&_frameMutex) != SCE_OK)
		return false;

	if (_videoFrame.pData == nullptr)
	{
		scePthreadMutexUnlock(&_frameMutex);
		return false;
	}

	void* lumaAddress;
	void* chromaAddress;
	Gnm::Texture lumaTexture;
	Gnm::Texture chromaTexture;

	auto frameWidth = _videoFrame.details.video.width;
	auto frameHeight = _videoFrame.details.video.height;
	auto framePitch = _videoFrame.details.video.pitch;

	// Set texture
	Gnm::SizeAlign sz;

	sz = lumaTexture.initAs2d(framePitch, frameHeight, 1, Gnm::kDataFormatR8Unorm, Gnm::kTileModeDisplay_LinearAligned, Gnm::kNumSamples1);
	assert(sz.m_size == framePitch * frameHeight);
	lumaAddress = _videoFrame.pData;
	lumaTexture.setBaseAddress256ByteBlocks((uint32_t)(reinterpret_cast<uint64_t>(lumaAddress) >> 8));

	sz = chromaTexture.initAs2d(framePitch / 2, frameHeight / 2, 1, Gnm::kDataFormatR8G8Unorm, Gnm::kTileModeDisplay_LinearAligned, Gnm::kNumSamples1);
	assert(sz.m_size == framePitch * (frameHeight / 2));
	chromaAddress = (uint8_t*)(_videoFrame.pData) + (framePitch * frameHeight);
	chromaTexture.setBaseAddress256ByteBlocks((uint32_t)(reinterpret_cast<uint64_t>(chromaAddress) >> 8));

	// Draw
	auto left = (framePitch > 0) ? ((float)_videoFrame.details.video.cropLeftOffset / framePitch) : 0.0f;
	auto right = (framePitch > 0) ? 1.0f - ((float)_videoFrame.details.video.cropRightOffset / framePitch) : 1.0f;
	auto top = (frameHeight > 0) ? ((float)_videoFrame.details.video.cropTopOffset / frameHeight) : 0.0f;
	auto bottom = (frameHeight > 0) ? 1.0f - ((float)_videoFrame.details.video.cropBottomOffset / frameHeight) : 1.0f;
	_graphics->DrawYCbCr(&lumaTexture, &chromaTexture, left, right, top, bottom);

	memset(&_videoFrame, 0, sizeof(SceAvPlayerFrameInfoEx));
	scePthreadMutexUnlock(&_frameMutex);
	return true;
}

void VideoPlayer::Pause()
{
	if (_state != VideoPlayerState::Playing)
		return;

	auto ret = sceAvPlayerPause(_handle);
	assert(ret == SCE_OK);
	if (ret == SCE_OK)
		_state = VideoPlayerState::Paused;
}

void VideoPlayer::Resume()
{
	if (_state != VideoPlayerState::Paused)
		return;

	auto ret = sceAvPlayerResume(_handle);
	assert(ret == SCE_OK);
	if (ret == SCE_OK)
		_state = VideoPlayerState::Playing;
}

void VideoPlayer::Play(const char* filename)
{
    switch(_state)
	{
	case VideoPlayerState::Not_Loaded:
		{
			_sourceID = sceAvPlayerAddSource(_handle, filename);
			sceAvPlayerSetLooping(_handle, _isLooped);

			ScePthreadAttr threadAttr;
			scePthreadAttrInit(&threadAttr);
			scePthreadAttrSetstacksize(&threadAttr, 1024 * 1024);
			scePthreadAttrSetaffinity(&threadAttr, (1 << 4) | (1 << 5));
			auto ret = scePthreadCreate(&_videoThread, &threadAttr, videoOutputThread, this, "av_video_output_thread");
			scePthreadAttrDestroy(&threadAttr);
			if (ret < 0)
			{
				printf("Couldn't start video thread: 0x%08X\n", ret);
				return;
			}

			scePthreadAttrInit(&threadAttr);
			scePthreadAttrSetstacksize(&threadAttr, 1024 * 1024);
			scePthreadAttrSetaffinity(&threadAttr, (1 << 4) | (1 << 5));
			ret = scePthreadCreate(&_audioThread, &threadAttr, audioOutputThread, this, "av_audio_output_thread");
			scePthreadAttrDestroy(&threadAttr);
			if (ret < 0)
			{
				printf("Couldn't start audio thread: 0x%08X\n", ret);
				return;
			}

			_state = VideoPlayerState::Playing;
		}
		break;

	case VideoPlayerState::Playing:
		sceAvPlayerJumpToTime(_handle, 0);
		return;

	case VideoPlayerState::Stopped:
		sceAvPlayerResume(_handle);
		sceAvPlayerJumpToTime(_handle, 0);
		_state = VideoPlayerState::Playing;
		break;
	}
}

void VideoPlayer::Stop()
{
	if (_state == VideoPlayerState::Stopped)
		return;

	if (_state != VideoPlayerState::Playing && _state != VideoPlayerState::Paused)
		return;

	auto ret = sceAvPlayerPause(_handle);
	assert(ret == SCE_OK);
	if (ret == SCE_OK)
		_state = VideoPlayerState::Stopped;
}

void VideoPlayer::SetVolume(float volume)
{
	scePthreadMutexLock(&_audioMutex);
	_volume = volume;
	scePthreadMutexUnlock(&_audioMutex);
}

time_t VideoPlayer::GetPlayPosition()
{
	return sceAvPlayerCurrentTime(_handle);
}

void VideoPlayer::SetIsLooped(bool value)
{
	_isLooped = value;

	if (_state != VideoPlayerState::Not_Loaded)
	{
		auto ret = sceAvPlayerSetLooping(_handle, value);
		assert(ret == SCE_OK);
	}
}

void VideoPlayer::SetIsMuted(bool value)
{
	scePthreadMutexLock(&_audioMutex);
	_isMuted = value;
	scePthreadMutexUnlock(&_audioMutex);
}
