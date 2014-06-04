#include "videoPlayer.h"
#include "../allocator.h"

#include <assert.h>
#include <memory>
#include <thread>
#include <libsysmodule.h>
#include <sceerror.h>

using namespace Media;

namespace {
	void* videoAlloc(void* argP, uint32_t argAlignment, uint32_t argSize)
	{
	}

	void videoFree(void* argP, void* argMemory)
	{
	}

	void* videoAllocTexture(void* argP, uint32_t argAlignment, uint32_t argSize)
	{
	}

	void videoFreeTexture(void* argP, void* argMemory)
	{
	}

	int openFile(void* argP, const char* argFilename)
	{
	}

	int closeFile(void* argP)
	{
	}

	int readOffsetFile(void* argP, uint8_t* argBuffer, uint64_t argPosition, uint32_t argLength)
	{
	}

	uint64_t sizeFile(void* argP)
	{
	}

	void eventCallback(void* p, int32_t argEventId, int32_t argSourceId, void* argEventData)
	{
	}
}

VideoPlayer::VideoPlayer()
{
	if (sceSysmoduleIsLoaded(SCE_SYSMODULE_AV_PLAYER) != SCE_SYSMODULE_LOADED)
	{
		auto ret = sceSysmoduleLoadModule(SCE_SYSMODULE_AV_PLAYER);
		assert(ret == SCE_OK);
	}

	SceAvPlayerInitData param;
	memset(&param, 0, sizeof(SceAvPlayerInitData));
	param.memoryReplacement.objectPointer = NULL;
	param.memoryReplacement.allocate = videoAlloc;
	param.memoryReplacement.deallocate = videoFree;
	param.memoryReplacement.allocateTexture = videoAllocTexture;
	param.memoryReplacement.deallocateTexture = videoFreeTexture;

	param.fileReplacement.objectPointer = NULL;
	param.fileReplacement.open = openFile;
	param.fileReplacement.close = closeFile;
	param.fileReplacement.readOffset = readOffsetFile;
	param.fileReplacement.size = sizeFile;

	param.eventReplacement.objectPointer = NULL;
	param.eventReplacement.eventCallback = eventCallback;

	param.debugLevel = SCE_AVPLAYER_DBG_ALL;
	param.basePriority = 160;
	param.numOutputVideoFrameBuffers = 2;
	param.autoStart = true;
	param.defaultLanguage = "eng";

	_handle = sceAvPlayerInit(&param);
}

VideoPlayer::~VideoPlayer()
{
}

Graphics::Texture* VideoPlayer::GetTexture()
{
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
