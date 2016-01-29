#pragma once

#include "predecls.h"
#include <scebase.h>
#include <np.h>
#include <np/np_common.h>

namespace Network {

enum class NpResult
{
	Ok = SCE_OK,

	ErrorInvalidArgument = SCE_NP_ERROR_INVALID_ARGUMENT,
	ErrorUnknownPlatformType = SCE_NP_ERROR_UNKNOWN_PLATFORM_TYPE,
	ErrorOutOfMemory = SCE_NP_ERROR_OUT_OF_MEMORY,
	ErrorSignedOut = SCE_NP_ERROR_SIGNED_OUT,
	ErrorUserNotFound = SCE_NP_ERROR_USER_NOT_FOUND,
	ErrorCallbackAlreadyRegistered = SCE_NP_ERROR_CALLBACK_ALREADY_REGISTERED,
	ErrorCallbackNotRegistered = SCE_NP_ERROR_CALLBACK_NOT_REGISTERED,
	ErrorNotSignedUp = SCE_NP_ERROR_NOT_SIGNED_UP,
	ErrorAgeRestriction = SCE_NP_ERROR_AGE_RESTRICTION,
	ErrorLogout = SCE_NP_ERROR_LOGOUT,
	ErrorLatestSystemSoftwareExist = SCE_NP_ERROR_LATEST_SYSTEM_SOFTWARE_EXIST,
	ErrorLatestSystemSoftwareExistForTitle = SCE_NP_ERROR_LATEST_SYSTEM_SOFTWARE_EXIST_FOR_TITLE,
	ErrorLatestPatchPkgExist = SCE_NP_ERROR_LATEST_PATCH_PKG_EXIST,
	ErrorLatestPatchPkgDownloaded = SCE_NP_ERROR_LATEST_PATCH_PKG_DOWNLOADED,
	ErrorInvalidSize = SCE_NP_ERROR_INVALID_SIZE,
	ErrorAborted = SCE_NP_ERROR_ABORTED,
	RequestMax = SCE_NP_ERROR_REQUEST_MAX,
	RequestNotFound = SCE_NP_ERROR_REQUEST_NOT_FOUND,
	InvalidId = SCE_NP_ERROR_INVALID_ID,
	UtilErrorInvalidNpId = SCE_NP_UTIL_ERROR_INVALID_NP_ID,
	UtilErrorNotMatch = SCE_NP_UTIL_ERROR_NOT_MATCH
};


CS_FLAGS enum class NpPlusFeature : uint64_t
{
	RealtimeMultiplay = SCE_NP_PLUS_FEATURE_REALTIME_MULTIPLAY,
	AsyncMultiplay = SCE_NP_PLUS_FEATURE_ASYNC_MULTIPLAY,
};

class CS_API Np
{
	Np();

public:
	static NpResult _SetNpTitleId(const char* titleId, const uint8_t* titleSecret);

	static NpResult _NotifyPlusFeature(UserServiceUserId userId, NpPlusFeature features);

	static NpResult _CheckPlus(UserServiceUserId userId, NpPlusFeature features, bool *authorised);

	// TODO: add support for setting array of SceNpContentRestriction for region specific restrictions
	static NpResult _SetContentRestriction(int defaultAgeRestriction);

	static NpResult _CheckNpAvailability(UserServiceUserId userId);

	static NpResult _GetParentalControlInfo(UserServiceUserId userId, int* age, bool *chatRestriction, bool* ugcRestriction);

	static NpResult _GetState(UserServiceUserId userId, int* state);
};

} // namespace Network
