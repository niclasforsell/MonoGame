#include "remotePlay.h"
#include "../allocator.h"

#include <libsysmodule.h>
#include <remoteplay.h>

using namespace System;

namespace {
	void* heap = nullptr;
}

RemotePlayResult RemotePlay::Initialize()
{
	int ret = -1;
	heap = Allocator::Get()->allocate(SCE_REMOTEPLAY_HEAP_SIZE);
	if (heap == nullptr)
		return RemotePlayResult::ErrorOutOfMemory;

	if (sceSysmoduleIsLoaded(SCE_SYSMODULE_REMOTE_PLAY) == SCE_SYSMODULE_ERROR_UNLOADED) {
		ret = sceSysmoduleLoadModule(SCE_SYSMODULE_REMOTE_PLAY);
		if (ret != SCE_OK) {
			return (RemotePlayResult)ret;
		}
	}

	ret = sceRemoteplayInitialize(heap, SCE_REMOTEPLAY_HEAP_SIZE);
	if (ret != SCE_OK) {
		return (RemotePlayResult)ret;
	}

	return RemotePlayResult::Ok;
}

RemotePlayResult RemotePlay::Terminate()
{
	auto ret = sceRemoteplayTerminate();
	if (ret != SCE_OK) {
		return (RemotePlayResult)ret;
	}

	if (heap != nullptr) {
		Allocator::Get()->release(heap);
		heap = nullptr;
	}

	if (sceSysmoduleIsLoaded(SCE_SYSMODULE_REMOTE_PLAY) == SCE_OK) {
		ret = sceSysmoduleUnloadModule(SCE_SYSMODULE_REMOTE_PLAY);
		if (ret != SCE_OK) {
			return (RemotePlayResult)ret;
		}
	}

	return RemotePlayResult::Ok;
}

RemotePlayResult RemotePlay::Approve()
{
	return (RemotePlayResult)sceRemoteplayApprove();
}

RemotePlayResult RemotePlay::Prohibit()
{
	return (RemotePlayResult)sceRemoteplayProhibit();
}

#if SCE_ORBIS_SDK_VERSION >= 0x01700081u // SDK Version 1.7
RemotePlayConnectionStatus RemotePlay::GetConnectionStatus(SceUserServiceUserId userId)
{
	int status;
	auto ret = sceRemoteplayGetConnectionStatus(userId, &status);
	if (ret != SCE_OK)
		return (RemotePlayConnectionStatus)ret;

	return (RemotePlayConnectionStatus)status;
}
#endif
