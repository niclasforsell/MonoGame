#include "videoRecording.h"
#include "../allocator.h"
#include <assert.h>
#include <memory>
#include <scebase.h>
#include <libsysmodule.h>

using namespace System;

namespace {
    void* heap = nullptr;
}

VideoRecordingResult VideoRecording::Open(const char* videoFileName, ThreadPrio threadPriority, time_t bufferSeconds)
{
	if (heap != nullptr)
	{
		Allocator::Get()->release(heap);
		heap = nullptr;
	}

	if (sceSysmoduleIsLoaded(SCE_SYSMODULE_VIDEO_RECORDING) != SCE_SYSMODULE_LOADED)
	{
		auto ret = sceSysmoduleLoadModule(SCE_SYSMODULE_VIDEO_RECORDING);
		assert(ret == SCE_OK);
	}

	auto pathLen = strlen(SCE_VIDEO_RECORDING_PATH_GAME);
	auto fileNameLen = strlen(videoFileName);
	auto maxLen = FILENAME_MAX - fileNameLen - pathLen;
	if (maxLen < 0)
		return VideoRecordingResult::ErrorInvalidValue;

	SceVideoRecordingParam param;
	memset(&param, 0, sizeof(SceVideoRecordingParam));
	param.size = sizeof(SceVideoRecordingParam);
	param.threadPrio = (int32_t)threadPriority;
	param.ringSec = bufferSeconds;
	sceVideoRecordingParamInit(&param);

	auto heapSize = sceVideoRecordingQueryMemSize(&param);
	if (heapSize < 0)
		return (VideoRecordingResult)heapSize;

	heap = Allocator::Get()->allocate(heapSize);

	char videoPath[FILENAME_MAX];
	memset(videoPath, 0, FILENAME_MAX);
	strncpy(videoPath, SCE_VIDEO_RECORDING_PATH_GAME, pathLen);
	strncat(videoPath, videoFileName, fileNameLen);

	return (VideoRecordingResult)sceVideoRecordingOpen(videoPath, &param, heap, heapSize);
}

VideoRecordingResult VideoRecording::Start()
{
	return (VideoRecordingResult)sceVideoRecordingStart();
}

VideoRecordingResult VideoRecording::Stop()
{
	return (VideoRecordingResult)sceVideoRecordingStop();
}

VideoRecordingResult VideoRecording::Close(bool discardRecording)
{
	auto ret = (VideoRecordingResult)sceVideoRecordingClose(discardRecording);

	Allocator::Get()->release(heap);
	heap = nullptr;

	return ret;
}

VideoRecordingResult VideoRecording::GetStatus()
{
	return (VideoRecordingResult)sceVideoRecordingGetStatus();
}
