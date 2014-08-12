#pragma once

#include "predecls.h"

#include <system_service.h>
#include <sceerror.h>

namespace System {

enum class SystemServiceResult
{
	Ok = SCE_OK,

	ErrorInternal = SCE_SYSTEM_SERVICE_ERROR_INTERNAL,
	ErrorUnavailable = SCE_SYSTEM_SERVICE_ERROR_UNAVAILABLE,
	ErrorParameter = SCE_SYSTEM_SERVICE_ERROR_PARAMETER,
	ErrorNoEvent = SCE_SYSTEM_SERVICE_ERROR_NO_EVENT,
	ErrorRejected = SCE_SYSTEM_SERVICE_ERROR_REJECTED,
	ErrorNeedDisplaySafeAreaSettings = SCE_SYSTEM_SERVICE_ERROR_NEED_DISPLAY_SAFE_AREA_SETTINGS,

};

enum class SystemLang
{
	Japanese = SCE_SYSTEM_PARAM_LANG_JAPANESE,
	English_UnitedStates = SCE_SYSTEM_PARAM_LANG_ENGLISH_US,
	French = SCE_SYSTEM_PARAM_LANG_FRENCH,
	Spanish = SCE_SYSTEM_PARAM_LANG_SPANISH,
	German = SCE_SYSTEM_PARAM_LANG_GERMAN,
	Italian = SCE_SYSTEM_PARAM_LANG_ITALIAN,
	Dutch = SCE_SYSTEM_PARAM_LANG_DUTCH,
	Portuguese_Portugal = SCE_SYSTEM_PARAM_LANG_PORTUGUESE_PT,
	Russian = SCE_SYSTEM_PARAM_LANG_RUSSIAN,
	Korean = SCE_SYSTEM_PARAM_LANG_KOREAN,
	Chinese_Traditional = SCE_SYSTEM_PARAM_LANG_CHINESE_T,
	Chinese_Simplified = SCE_SYSTEM_PARAM_LANG_CHINESE_S,
	Finnish = SCE_SYSTEM_PARAM_LANG_FINNISH,
	Swedish = SCE_SYSTEM_PARAM_LANG_SWEDISH,
	Danish = SCE_SYSTEM_PARAM_LANG_DANISH,
	Norwegian = SCE_SYSTEM_PARAM_LANG_NORWEGIAN,
	Polish = SCE_SYSTEM_PARAM_LANG_POLISH,
	Portuguese_Brazil = SCE_SYSTEM_PARAM_LANG_PORTUGUESE_BR,
	English_UnitedKingdom = SCE_SYSTEM_PARAM_LANG_ENGLISH_GB,
	Turkish = SCE_SYSTEM_PARAM_LANG_TURKISH,
	Spanish_LatinAmerica = SCE_SYSTEM_PARAM_LANG_SPANISH_LA,
};

enum class SystemDateFormat
{
	YearMonthDay = SCE_SYSTEM_PARAM_DATE_FORMAT_YYYYMMDD,
	DayMonthYear = SCE_SYSTEM_PARAM_DATE_FORMAT_DDMMYYYY,
	MonthDayYear = SCE_SYSTEM_PARAM_DATE_FORMAT_MMDDYYYY,
};

enum class SystemTimeFormat
{
	TwelveHour = SCE_SYSTEM_PARAM_TIME_FORMAT_12HOUR,
	TwentyFourHour = SCE_SYSTEM_PARAM_TIME_FORMAT_24HOUR,
};

enum class GpuLoadEmulationMode
{
	Off = SCE_SYSTEM_SERVICE_GPU_LOAD_EMULATION_MODE_OFF,
	Normal = SCE_SYSTEM_SERVICE_GPU_LOAD_EMULATION_MODE_NORMAL,
	High = SCE_SYSTEM_SERVICE_GPU_LOAD_EMULATION_MODE_HIGH,
};

enum class SystemServiceEvent
{
	OnResume = SCE_SYSTEM_SERVICE_EVENT_ON_RESUME,
	GameLiveStreamingStatusUpdate = SCE_SYSTEM_SERVICE_EVENT_GAME_LIVE_STREAMING_STATUS_UPDATE,
	SessionInvitation = SCE_SYSTEM_SERVICE_EVENT_SESSION_INVITATION,
	EntitlementUpdate = SCE_SYSTEM_SERVICE_EVENT_ENTITLEMENT_UPDATE,
	GameCustomData = SCE_SYSTEM_SERVICE_EVENT_GAME_CUSTOM_DATA,
	DisplaySafeAreaUpdate = SCE_SYSTEM_SERVICE_EVENT_DISPLAY_SAFE_AREA_UPDATE,
};


class CS_API SystemService
{
	SystemService() { }

protected:

	static int32_t GetNumRecieveEvents();

	static bool ReceiveEvent(CS_OUT SystemServiceEvent *eventType);

public:

	static SystemLang GetLanguage();

	static SystemDateFormat GetDateFormat(); 

	static SystemTimeFormat GetTimeFormat();

	static int32_t GetTimeZoneOffset();

	static bool GetIsDaylightSavingsTime();

	static SystemServiceResult LoadExec(const char *path, char* const argv[]);

	static void HideSplashScreen();

	static void DisableMusicPlayer();

	static void ReenableMusicPlayer();

	static void ShowDisplaySafeAreaSettings();

	static SystemServiceResult GetDisplaySafeAreaInfo(CS_OUT float *ratio);

	static bool GetIsSystemUIOverlaid();

	static bool GetIsInBackgroundExecution();

	static GpuLoadEmulationMode GetGpuLoadEmulationMode();

	static void SetGpuLoadEmulationMode(GpuLoadEmulationMode mode);
};

} // namespace System
