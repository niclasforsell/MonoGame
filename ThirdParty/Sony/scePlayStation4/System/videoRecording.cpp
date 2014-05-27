#include "videoRecording.h"
#include "../allocator.h"
#include <assert.h>
#include <memory>
#include <scebase.h>
#include <libsysmodule.h>

using namespace System;

namespace {
	void* heap = nullptr;

	// CppSharp gives us ASCII UTF-8 with its string wrapping, but video recording
	// expects a UTF-16 string in big endian byte order.
	bool ConvertUtf8ToUtf16be(const char* input, uint16_t* output, size_t maxOutput, uint32_t* outputLen)
	{
		SceCesUcsContext cesContext;
		auto ret = sceCesUcsContextInit(&cesContext);
		if (ret != SCE_OK)
			return false;

		ret = sceCesSetUtf16StrEndian(&cesContext, SCE_CES_ENDIAN_SYS, SCE_CES_ENDIAN_BE);
		if (ret != SCE_OK)
			return false;

		auto inputLength = strlen(input);
		auto utf16Value = new uint16_t[maxOutput];
		memset(utf16Value, 0, maxOutput);
		uint32_t utf8Len = 0;
		ret = sceCesUtf8StrToUtf16Str(&cesContext, (const uint8_t*)input, inputLength, &utf8Len, utf16Value, 256, outputLen);
		if (ret != SCE_OK)
			return false;

		return true;
	}
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

VideoRecordingResult VideoRecording::MarkChapterChange()
{
	int32_t value = SCE_VIDEO_RECORDING_CHAPTER_CHANGE;
	auto ret = sceVideoRecordingSetInfo(SCE_VIDEO_RECORDING_INFO_CHAPTER, &value, sizeof(int32_t));
	if (ret < 0)
		return (VideoRecordingResult)ret;

	return VideoRecordingResult::Ok;
}

VideoRecordingResult VideoRecording::ProhibitRecording()
{
	int32_t value = SCE_VIDEO_RECORDING_CHAPTER_PROHIBIT;
	auto ret = sceVideoRecordingSetInfo(SCE_VIDEO_RECORDING_INFO_CHAPTER, &value , sizeof(int32_t));
	if (ret < 0)
		return (VideoRecordingResult)ret;

	return VideoRecordingResult::Ok;;
}

// UTF-16BE up to 256 bytes
VideoRecordingResult VideoRecording::SetSubtitle(const char* value)
{
	uint16_t buffer[256];
	uint32_t outputLen;
	if(!ConvertUtf8ToUtf16be(value, buffer, 256, &outputLen))
		return VideoRecordingResult::ErrorStringConversionFailed;

	auto ret = sceVideoRecordingSetInfo(SCE_VIDEO_RECORDING_INFO_SUBTITLE, buffer, outputLen);
	if (ret < 0)
		return (VideoRecordingResult)ret;

	return VideoRecordingResult::Ok;
}

// UTF-16BE up to 2000 bytes
VideoRecordingResult VideoRecording::SetDescription(const char* value)
{
	uint16_t buffer[2000];
	uint32_t outputLen;
	if(!ConvertUtf8ToUtf16be(value, buffer, 2000, &outputLen))
		return VideoRecordingResult::ErrorStringConversionFailed;

	auto ret = sceVideoRecordingSetInfo(SCE_VIDEO_RECORDING_INFO_DESCRIPTION, buffer, outputLen);
	if (ret < 0)
		return (VideoRecordingResult)ret;

	return VideoRecordingResult::Ok;
}

// UTF-16BE up to 512 bytes
VideoRecordingResult VideoRecording::SetComments(const char* value)
{
	uint16_t buffer[512];
	uint32_t outputLen;
	if (!ConvertUtf8ToUtf16be(value, buffer, 512, &outputLen))
		return VideoRecordingResult::ErrorStringConversionFailed;

	auto ret = sceVideoRecordingSetInfo(SCE_VIDEO_RECORDING_INFO_COMMENTS, buffer, outputLen);
	if (ret < 0)
		return (VideoRecordingResult)ret;

	return VideoRecordingResult::Ok;
}

// UTF-16BE up to 256 bytes
VideoRecordingResult VideoRecording::SetKeywords(const char* value)
{
	uint16_t buffer[256];
	uint32_t outputLen;
	if (!ConvertUtf8ToUtf16be(value, buffer, 256, &outputLen))
		return VideoRecordingResult::ErrorStringConversionFailed;

	auto ret = sceVideoRecordingSetInfo(SCE_VIDEO_RECORDING_INFO_KEYWORDS, buffer, outputLen);
	if (ret < 0)
		return (VideoRecordingResult)ret;

	return VideoRecordingResult::Ok;
}

// UTF-16BE up to 2000 bytes
VideoRecordingResult VideoRecording::SetCopyright(const char* value)
{
	uint16_t buffer[2000];
	uint32_t outputLen;
	if (!ConvertUtf8ToUtf16be(value, buffer, 2000, &outputLen))
		return VideoRecordingResult::ErrorStringConversionFailed;

	auto ret = sceVideoRecordingSetInfo(SCE_VIDEO_RECORDING_INFO_COPYRIGHT, buffer, outputLen);
	if (ret < 0)
		return (VideoRecordingResult)ret;

	return VideoRecordingResult::Ok;
}

VideoRecordingResult VideoRecording::SetPermissionLevel(int32_t permissionLevel)
{
	return (VideoRecordingResult)sceVideoRecordingSetInfo(SCE_VIDEO_RECORDING_INFO_PERMISSION_LEVEL, &permissionLevel, sizeof(int32_t));
}
