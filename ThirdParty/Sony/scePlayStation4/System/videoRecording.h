#pragma once
#include "predecls.h"
#include "threadPrio.h"
#include <scebase.h>
#include <video_recording.h>
#include <ces.h>

namespace System {

enum class VideoRecordingResult
{
	Ok = SCE_OK,

	StatusNone = SCE_VIDEO_RECORDING_STATUS_NONE,
	StatusRunning = SCE_VIDEO_RECORDING_STATUS_RUNNING,
	StatusPaused = SCE_VIDEO_RECORDING_STATUS_PAUSED,

	ErrorOutOfMemory = SCE_VIDEO_RECORDING_ERROR_OUT_OF_MEMORY,
	ErrorFatal = SCE_VIDEO_RECORDING_ERROR_FATAL,
	ErrorInvalidValue = SCE_VIDEO_RECORDING_ERROR_INVALID_VALUE,
	ErrorFileOpen = SCE_VIDEO_RECORDING_ERROR_FILE_OPEN,
	ErrorFileWrite = SCE_VIDEO_RECORDING_ERROR_FILE_WRITE,
	ErrorInvalidState = SCE_VIDEO_RECORDING_ERROR_INVALID_STATE,
	ErrorInterrupted = SCE_VIDEO_RECORDING_ERROR_INTERRUPTED,
	ErrorNoSpace = SCE_VIDEO_RECORDING_ERROR_NO_SPACE,

#if SCE_ORBIS_SDK_VERSION >= 0x01700081u // SDK Version 1.7
	ErrorFileNoData = SCE_VIDEO_RECORDING_ERROR_FILE_NO_DATA
#endif

	ErrorStringConversionFailed
};

class CS_API VideoRecording
{
	VideoRecording() {}

public:

	static VideoRecordingResult Open(const char* videoFileName, ThreadPrio threadPriority, time_t bufferSeconds);

	static VideoRecordingResult Start();

	static VideoRecordingResult Stop();

	static VideoRecordingResult Close(bool discardRecording);

	static VideoRecordingResult GetStatus();

	static VideoRecordingResult MarkChapterChange();
	static VideoRecordingResult ProhibitRecording();

	// Metadata
	static VideoRecordingResult SetSubtitle(const char* value);
	static VideoRecordingResult SetDescription(const char* value);
	static VideoRecordingResult SetComments(const char* value);
	static VideoRecordingResult SetKeywords(const char* value);
	static VideoRecordingResult SetCopyright(const char* value);
	static VideoRecordingResult SetPermissionLevel(int32_t permissionLevel);
};

} // namespace System
