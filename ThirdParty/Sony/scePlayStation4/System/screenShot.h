#pragma once

#include "predecls.h"

#include <screenshot.h>
#include <sceerror.h>

namespace System {

enum class ScreenShotError
{
	Ok = SCE_OK,

	InvalidArgument = SCE_SCREENSHOT_ERROR_INVALID_ARGUMENT,
	NoMemory = SCE_SCREENSHOT_ERROR_NO_MEMORY,
	FileNotFound = SCE_SCREENSHOT_ERROR_FILE_NOT_FOUND,
	NotSupportedFormat = SCE_SCREENSHOT_ERROR_NOT_SUPPORTED_FORMAT,
	InternalError = SCE_SCREENSHOT_ERROR_INTERNAL,
	NotSupportedData = SCE_SCREENSHOT_ERROR_NOT_SUPPORTED_DATA,	
};

class CS_API ScreenShot
{
	static void _tryInitialize();

	static bool _enabled;

	ScreenShot() { }

public:

	static bool GetIsEnabled();

	static void SetIsEnabled(bool enable);

	static ScreenShotError SetOverlayImage(const char *filePath, int32_t offsetX, int32_t offsetY);

	static ScreenShotError SetParam(const char *photoTitle, const char *gameTitle, const char *gameComment);

};

} // namespace System
