#pragma once

#include "predecls.h"
#include <scebase.h>
#include <np.h>

namespace Network {

enum class TrophyResult
{
	Ok = SCE_OK,

	ErrorUnknown = SCE_NP_TROPHY_ERROR_UNKNOWN,
	ErrorNotInitialized = SCE_NP_TROPHY_ERROR_NOT_INITIALIZED,
	ErrorAlreadyInitialized = SCE_NP_TROPHY_ERROR_ALREADY_INITIALIZED,
	ErrorOutOfMemory = SCE_NP_TROPHY_ERROR_OUT_OF_MEMORY,
	ErrorInvalidArgument = SCE_NP_TROPHY_ERROR_INVALID_ARGUMENT,
	ErrorInsufficientBuffer = SCE_NP_TROPHY_ERROR_INSUFFICIENT_BUFFER,
	ErrorExceedsMax = SCE_NP_TROPHY_ERROR_EXCEEDS_MAX,
	ErrorAbort = SCE_NP_TROPHY_ERROR_ABORT,
	ErrorInvalidHandle = SCE_NP_TROPHY_ERROR_INVALID_HANDLE,
	ErrorInvalidContext = SCE_NP_TROPHY_ERROR_INVALID_CONTEXT,
	ErrorInvalidTrophyId = SCE_NP_TROPHY_ERROR_INVALID_TROPHY_ID,
	ErrorInvalidGroupId = SCE_NP_TROPHY_ERROR_INVALID_GROUP_ID,
	ErrorTrophyAlreadyUnlocked = SCE_NP_TROPHY_ERROR_TROPHY_ALREADY_UNLOCKED,
	ErrorPlatinumCannotUnlock = SCE_NP_TROPHY_ERROR_PLATINUM_CANNOT_UNLOCK,
	ErrorAccountIdNotMatch = SCE_NP_TROPHY_ERROR_ACCOUNTID_NOT_MATCH,
	ErrorNotRegistered = SCE_NP_TROPHY_ERROR_NOT_REGISTERED,
	ErrorAlreadyRegistered = SCE_NP_TROPHY_ERROR_ALREADY_REGISTERED,
	ErrorBrokenData = SCE_NP_TROPHY_ERROR_BROKEN_DATA,
	ErrorInsufficientSpace = SCE_NP_TROPHY_ERROR_INSUFFICIENT_SPACE,
	ErrorContextAlreadyExists = SCE_NP_TROPHY_ERROR_CONTEXT_ALREADY_EXISTS,
	ErrorIconFileNotFound = SCE_NP_TROPHY_ERROR_ICON_FILE_NOT_FOUND,
	ErrorInvalidTrpFileFormat = SCE_NP_TROPHY_ERROR_INVALID_TRP_FILE_FORMAT,
	ErrorUnsupportedTrpFile = SCE_NP_TROPHY_ERROR_UNSUPPORTED_TRP_FILE,
	ErrorInvalidTrophyConfFormat = SCE_NP_TROPHY_ERROR_INVALID_TROPHY_CONF_FORMAT,
	ErrorUnsupportedTrophyConf = SCE_NP_TROPHY_ERROR_UNSUPPORTED_TROPHY_CONF,
	ErrorTrophyNotUnlocked = SCE_NP_TROPHY_ERROR_TROPHY_NOT_UNLOCKED,
	ErrorUserNotFound = SCE_NP_TROPHY_ERROR_USER_NOT_FOUND,
	ErrorUserNotLoggedIn = SCE_NP_TROPHY_ERROR_USER_NOT_LOGGED_IN,
	ErrorContextUserLogout = SCE_NP_TROPHY_ERROR_CONTEXT_USER_LOGOUT,
	ErrorUseTrpForDevelopment = SCE_NP_TROPHY_ERROR_USE_TRP_FOR_DEVELOPMENT,
	ErrorInvalidNpTitleId = SCE_NP_TROPHY_ERROR_INVALID_NP_TITLE_ID,
	ErrorInvalidNpServiceLabel = SCE_NP_TROPHY_ERROR_INVALID_NP_SERVICE_LABEL,
	ErrorNotSupported = SCE_NP_TROPHY_ERROR_NOT_SUPPORTED,
	ErrorContextExceedsMax = SCE_NP_TROPHY_ERROR_CONTEXT_EXCEEDS_MAX,
	ErrorHandleExceedsMax = SCE_NP_TROPHY_ERROR_HANDLE_EXCEEDS_MAX,
	ErrorInvalidUserId = SCE_NP_TROPHY_ERROR_INVALID_USER_ID,
	ErrorTitleConfNotInstalled = SCE_NP_TROPHY_ERROR_TITLE_CONF_NOT_INSTALLED,
	ErrorBrokenTitleCOnf = SCE_NP_TROPHY_ERROR_BROKEN_TITLE_CONF
#if SCE_ORBIS_SDK_VERSION >= 0x01700081u // SDK Version 1.7
	,
	ErrorInconsistentTitleConf = SCE_NP_TROPHY_ERROR_INCONSISTENT_TITLE_CONF
#endif
};

class CS_API TrophySet
{
	CS_IGNORE SceNpTrophyContext _context;

protected:
	TrophySet();
	TrophyResult CreateContext(const SceUserServiceUserId userId, const SceNpServiceLabel serviceLabel);

public:
	TrophySet(const TrophySet&) = delete;
	~TrophySet();

	TrophyResult Unlock(int32_t trophyId, CS_OUT bool* platinumReceived);
};

} // namespace Network
