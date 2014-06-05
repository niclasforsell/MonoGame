#include "videoPlayer.h"
#include "../allocator.h"
#include "../Graphics/vertexShader.h"
#include "../Graphics/pixelShader.h"
#include "../Graphics/graphicsEnums.h"
#include "../Graphics/graphicsSystem.h"
#include "../Graphics/renderTarget.h"
#include "../Graphics/texture.h"

#include <assert.h>
#include <memory>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <libsysmodule.h>
#include <sceerror.h>
#include <gnm.h>
#include <gnmx.h>
#include <gnmx/shader_parser.h>
#include <sceavplayer_ex.h>
#include <video_out.h>

using namespace sce;
using namespace Graphics;
using namespace Media;

namespace {
	void* videoAlloc(void* argP, uint32_t argAlignment, uint32_t argSize)
	{
		return Allocator::Get()->allocate(argSize, argAlignment, SCE_KERNEL_WB_ONION);
	}

	void videoFree(void* argP, void* argMemory)
	{
		Allocator::Get()->release(argP);
	}

	void* videoAllocTexture(void* argP, uint32_t argAlignment, uint32_t argSize)
	{
		return Allocator::Get()->allocate(argSize, argAlignment, SCE_KERNEL_WC_GARLIC);
	}

	void videoFreeTexture(void* argP, void* argMemory)
	{
		Allocator::Get()->release(argP);
	}

	void* videoOutputThread(void* arg)
	{
		auto player = (VideoPlayer*)arg;
		assert(player != nullptr);

		std::unique_lock<std::mutex> lock(player->_frameLock);

		player->_frameAvailable = false;

		for(;;)
		{
			player->_decodeReady.wait(lock, [player] { return !player->_frameAvailable; });
			if (sceAvPlayerGetVideoDataEx(player->_handle, &player->_videoFrame))
			{
				player->_frameAvailable = true;
				player->_displayReady.notify_one();
			}
		}

		scePthreadExit(NULL);
		return NULL;
	}

	void* audioOutputThread(void* arg)
	{
		auto player = (VideoPlayer*)arg;
		assert(player != nullptr);

		SceAvPlayerFrameInfo audioFrame;

		for(;;)
		{
			if (sceAvPlayerGetAudioData(player->_handle, &audioFrame))
			{
				// Do audio here
			}
			else
			{
				// Do silence here to prevent the thread from blocking
			}
		}
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
	memset(&_audioFrame, 0, sizeof(SceAvPlayerFrameInfoEx));

	SceAvPlayerInitData param;
	memset(&param, 0, sizeof(SceAvPlayerInitData));
	param.memoryReplacement.objectPointer = NULL;
	param.memoryReplacement.allocate = videoAlloc;
	param.memoryReplacement.deallocate = videoFree;
	param.memoryReplacement.allocateTexture = videoAllocTexture;
	param.memoryReplacement.deallocateTexture = videoFreeTexture;

	param.debugLevel = SCE_AVPLAYER_DBG_INFO;
	param.basePriority = 160;
	param.numOutputVideoFrameBuffers = 2;
	param.autoStart = true;
	param.defaultLanguage = "eng";

	_handle = sceAvPlayerInit(&param);

	_graphics = graphics;
}

VideoPlayer::~VideoPlayer()
{
	Stop();
	delete _videoThread;
	delete _audioThread;
}

void VideoPlayer::GrabFrame()
{
	std::unique_lock<std::mutex> lock(_frameLock);
	_displayReady.wait(lock, [this]() { return _frameAvailable; });

	void* lumaAddress;
	void* chromaAddress;
	Gnm::Texture lumaTexture;
	Gnm::Texture chromaTexture;

	auto frameWidth = _videoFrame.details.video.width;
	auto frameHeight = _videoFrame.details.video.height;
	auto framePitch = _videoFrame.details.video.pitch;

	// Set texture
	{
		Gnm::SizeAlign sz;

		sz = lumaTexture.initAs2d(framePitch, frameHeight, 1, Gnm::kDataFormatR8Unorm, Gnm::kTileModeDisplay_LinearAligned, Gnm::kNumSamples1);
		assert(sz.m_size == framePitch * frameHeight);
		lumaAddress = _videoFrame.pData;
		lumaTexture.setBaseAddress256ByteBlocks((uint32_t)(reinterpret_cast<uint64_t>(lumaAddress) >> 8));

		sz = chromaTexture.initAs2d(framePitch / 2, frameHeight / 2, 1, Gnm::kDataFormatR8G8Unorm, Gnm::kTileModeDisplay_LinearAligned, Gnm::kNumSamples1);
		assert(sz.m_size == framePitch * (frameHeight / 2));
		chromaAddress = (uint8_t*)(_videoFrame.pData) + (framePitch * frameHeight);
		chromaTexture.setBaseAddress256ByteBlocks((uint32_t)(reinterpret_cast<uint64_t>(chromaAddress) >> 8));
	}

	// Draw
	auto left = (framePitch > 0) ? ((float)_videoFrame.details.video.cropLeftOffset / framePitch) : 0.0f;
	auto right = (framePitch > 0) ? 1.0f - ((float)_videoFrame.details.video.cropRightOffset / framePitch) : 1.0f;
	auto top = (frameHeight > 0) ? ((float)_videoFrame.details.video.cropTopOffset / frameHeight) : 0.0f;
	auto bottom = (frameHeight > 0) ? 1.0f - ((float)_videoFrame.details.video.cropBottomOffset / frameHeight) : 1.0f;
	_graphics->DrawYCbCr(&lumaTexture, &chromaTexture, left, right, top, bottom);

	_frameAvailable = false;
	_decodeReady.notify_one();
}

void VideoPlayer::Pause()
{
	auto ret = sceAvPlayerPause(_handle);
	assert(ret == SCE_OK);
}

void VideoPlayer::Resume()
{
	auto ret = sceAvPlayerResume(_handle);
	assert(ret == SCE_OK);
}

void VideoPlayer::Play(const char* filename)
{
	_sourceID = sceAvPlayerAddSource(_handle, filename);

	// Temporary
	sceAvPlayerSetLooping(_handle, true);

	_videoThread = new std::thread(videoOutputThread, this);
	_audioThread = new std::thread(audioOutputThread, this);
}

void VideoPlayer::Stop()
{
	auto ret = sceAvPlayerStop(_handle);
	assert(ret == SCE_OK);
}

void VideoPlayer::SetVolume(float volume)
{
}

time_t VideoPlayer::GetPlayPosition()
{
	return sceAvPlayerCurrentTime(_handle) * 1000;
}
