#pragma once

#include "predecls.h"
#include <user_service.h>
#include <sceerror.h>
#include <remoteplay.h>

namespace System {

enum class RemotePlayResult
{
	Ok = SCE_OK,

	ErrorInvalidArgs = SCE_REMOTEPLAY_ERROR_INVALID_ARGS,
	ErrorOutOfMemory = SCE_REMOTEPLAY_ERROR_OUT_OF_MEMORY,
	ErrorAlreadyInitialized = SCE_REMOTEPLAY_ERROR_NOT_INITIALIZED
};

class CS_API RemotePlay
{
	RemotePlay() { }

public:
	static RemotePlayResult Initialize();
	static RemotePlayResult Terminate();

	static RemotePlayResult Update(float elapsedSeconds);

	static bool GetIsEnabled();
	static void SetIsEnabled(bool value);
};

} // namespace System
