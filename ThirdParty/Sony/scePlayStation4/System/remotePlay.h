#pragma once

#include "predecls.h"
#include <scebase.h>
#include <remoteplay.h>

namespace System {

typedef int32_t SceUserServiceUserId;

enum RemotePlayResult
{
	Ok = SCE_OK,

	ErrorInvalidArgs = SCE_REMOTEPLAY_ERROR_INVALID_ARGS,
	ErrorOutOfMemory = SCE_REMOTEPLAY_ERROR_OUT_OF_MEMORY,
	ErrorAlreadyInitialized = SCE_REMOTEPLAY_ERROR_NOT_INITIALIZED
};

#if SCE_ORBIS_SDK_VERSION >= 0x01700081u // SDK Version 1.7
enum RemotePlayConnectionStatus
{
	Disconnect = SCE_REMOTEPLAY_CONNECTION_STATUS_DISCONNECT,
	Connect = SCE_REMOTEPLAY_CONNECTION_STATUS_CONNECT,

	// This doesn't quite match the docs, but we don't appear to be able
	// to return enums with CS_OUT at this juncture.
	ErrorNotInitialized = SCE_REMOTEPLAY_ERROR_NOT_INITIALIZED
};
#endif

class CS_API RemotePlay
{
	RemotePlay() { }

public:
	static RemotePlayResult Initialize();

	static RemotePlayResult Terminate();

	static RemotePlayResult Approve();

	static RemotePlayResult Prohibit();

#if SCE_ORBIS_SDK_VERSION >= 0x01700081u // SDK Version 1.7
	static RemotePlayConnectionStatus GetConnectionStatus(SceUserServiceUserId userId);
#endif
};

} // namespace System
