#pragma once

#include "predecls.h"
#include <scebase.h>
#include <np.h>

namespace Network {

enum class TitleStorageResult
{
	Ok = SCE_OK,

	ErrorNpInvalidArgument = SCE_NP_ERROR_INVALID_ARGUMENT,
	ErrorNpSignedOut = SCE_NP_ERROR_SIGNED_OUT,
	ErrorNpUserNotFound = SCE_NP_ERROR_USER_NOT_FOUND,
	ErrorNpUserNotSignedUp = SCE_NP_ERROR_NOT_SIGNED_UP,

	ErrorNotInitialized = SCE_NP_COMMUNITY_ERROR_NOT_INITIALIZED,
	ErrorOutOfMemory = SCE_NP_COMMUNITY_ERROR_OUT_OF_MEMORY,
	ErrorInvalidArgument = SCE_NP_COMMUNITY_ERROR_INVALID_ARGUMENT,
	ErrorTooManyObjects = SCE_NP_COMMUNITY_ERROR_TOO_MANY_OBJECTS,
	ErrorAborted = SCE_NP_COMMUNITY_ERROR_ABORTED,
	ErrorBodyTooLarge = SCE_NP_COMMUNITY_ERROR_BODY_TOO_LARGE,
	ErrorInsufficientArgument = SCE_NP_COMMUNITY_ERROR_INSUFFICIENT_ARGUMENT,
	ErrorInvalidId = SCE_NP_COMMUNITY_ERROR_INVALID_ID
};

enum class TitleStorageStatus
{
	Ok,
	Partial,
	NotModified
};

class CS_API TitleStorage
{
	CS_IGNORE SceNpId _npId;
	CS_IGNORE int32_t _titleCtxId;

protected:
	TitleStorage();
	TitleStorageResult CreateContext(const SceUserServiceUserId userId, const SceNpServiceLabel serviceLabel);
	TitleStorageResult GetData(SceNpTssSlotId slotId, void* buffer, size_t bufferSize, CS_OUT size_t* recvdSize);

public:
	TitleStorage(const TitleStorage&) = delete;
	~TitleStorage();
};

} // namespace Network
