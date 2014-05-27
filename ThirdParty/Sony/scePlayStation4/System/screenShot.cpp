#include "screenShot.h"

#include <memory>
#include <libsysmodule.h>


using namespace System;

bool ScreenShot::_enabled = true;

void ScreenShot::_tryInitialize()
{
	if (sceSysmoduleIsLoaded(SCE_SYSMODULE_SCREEN_SHOT) == SCE_SYSMODULE_LOADED)
		return;

	auto ret = sceSysmoduleLoadModule(SCE_SYSMODULE_SCREEN_SHOT);
	if (ret < 0)
	{
		printf("ERROR: Couldn't initialize screenshot library: 0x%08X\n", ret);
		return;
	}
}

bool ScreenShot::GetIsEnabled()
{
	return _enabled;
}

void ScreenShot::SetIsEnabled(bool enabled)
{
	_tryInitialize();

	if (enabled)
	{
		auto error = sceScreenShotEnable();
		_enabled = error == SCE_OK ? true : _enabled;
	}
	else
	{
		auto error = sceScreenShotDisable();
		_enabled = error == SCE_OK ? false : _enabled;
	}
}

ScreenShotError ScreenShot::SetOverlayImage(const char *filePath, int32_t offsetX, int32_t offsetY)
{
	_tryInitialize();

	auto error = sceScreenShotSetOverlayImage(filePath, offsetX, offsetY);
	return (ScreenShotError)error;
}

ScreenShotError ScreenShot::SetParam(const char *photoTitle, const char *gameTitle, const char *gameComment)
{
	_tryInitialize();

	SceScreenShotParam param;
	memset(&param, 0, sizeof(param));

	param.photoTitle = photoTitle;
	param.gameTitle = gameTitle;
	param.gameComment = gameComment;

	auto error = sceScreenShotSetParam(&param);
	return (ScreenShotError)error;
}
