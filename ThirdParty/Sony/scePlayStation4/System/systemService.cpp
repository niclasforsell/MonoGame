#include "systemService.h"

#include <memory.h>

using namespace System;


SystemLang SystemService::GetLanguage()
{
	int32_t language;
	auto result = sceSystemServiceParamGetInt(SCE_SYSTEM_SERVICE_PARAM_ID_LANG, &language);
	return (SystemLang)language;
}

SystemDateFormat SystemService::GetDateFormat()
{
	int32_t format;
	auto result = sceSystemServiceParamGetInt(SCE_SYSTEM_SERVICE_PARAM_ID_DATE_FORMAT, &format);
	return (SystemDateFormat)format;
}

SystemTimeFormat SystemService::GetTimeFormat()
{
	int32_t format;
	auto result = sceSystemServiceParamGetInt(SCE_SYSTEM_SERVICE_PARAM_ID_TIME_FORMAT, &format);
	return (SystemTimeFormat)format;
}

int32_t SystemService::GetTimeZoneOffset()
{
	int32_t offset;
	auto result = sceSystemServiceParamGetInt(SCE_SYSTEM_SERVICE_PARAM_ID_TIME_ZONE, &offset);
	return offset;
}

bool SystemService::GetIsDaylightSavingsTime()
{
	int32_t summertime;
	auto result = sceSystemServiceParamGetInt(SCE_SYSTEM_SERVICE_PARAM_ID_SUMMERTIME, &summertime);
	return summertime > 0 ? true : false;
}

SystemServiceResult SystemService::LoadExec(const char *path, char* const argv[])
{
	auto result = sceSystemServiceLoadExec(path, argv);
	return (SystemServiceResult)result;
}

void SystemService::HideSplashScreen()
{
	auto result = sceSystemServiceHideSplashScreen();
}

void SystemService::DisableMusicPlayer()
{
	auto result = sceSystemServiceDisableMusicPlayer();
}

void SystemService::ReenableMusicPlayer()
{
	auto result = sceSystemServiceReenableMusicPlayer();
}

void SystemService::ShowDisplaySafeAreaSettings()
{
	auto result = sceSystemServiceShowDisplaySafeAreaSettings();
}

SystemServiceResult SystemService::GetDisplaySafeAreaInfo(CS_OUT float *ratio)
{
	SceSystemServiceDisplaySafeAreaInfo info;
	memset(&info, 0, sizeof(info));
	auto result = sceSystemServiceGetDisplaySafeAreaInfo(&info);
	if (result == SCE_OK)
		*ratio = info.ratio;

	return (SystemServiceResult)result;
}

int32_t SystemService::GetNumRecieveEvents()
{
	SceSystemServiceStatus status;
	sceSystemServiceGetStatus(&status);
	return status.eventNum;
}

bool SystemService::ReceiveEvent(CS_OUT SystemServiceEvent *eventType)
{
	SceSystemServiceEvent event;
	auto result = sceSystemServiceReceiveEvent(&event);
	if (result != SCE_OK)
		return false;

	*eventType = (SystemServiceEvent)event.eventType;
	// TODO: Return data too!

	return true;
}

bool SystemService::GetIsSystemUIOverlaid()
{
	SceSystemServiceStatus status;
	sceSystemServiceGetStatus(&status);
	return status.isSystemUiOverlaid;
}

bool SystemService::GetIsInBackgroundExecution()
{
	SceSystemServiceStatus status;
	sceSystemServiceGetStatus(&status);
	return status.isInBackgroundExecution;
}

GpuLoadEmulationMode SystemService::GetGpuLoadEmulationMode()
{
	auto result = sceSystemServiceGetGpuLoadEmulationMode();
	return (GpuLoadEmulationMode)result;
}

void SystemService::SetGpuLoadEmulationMode(GpuLoadEmulationMode mode)
{
	auto result = sceSystemServiceSetGpuLoadEmulationMode((SceSystemServiceGpuLoadEmulationMode)mode);
}
