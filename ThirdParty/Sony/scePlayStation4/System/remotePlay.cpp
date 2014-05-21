#include "remotePlay.h"
#include "../allocator.h"

#include <libsysmodule.h>
#include <remoteplay.h>

using namespace System;

namespace {
	// While prohibiting remote play, the block must be periodically renewed within
	// a defined interval. We set the bar slightly lower to account for any delays.
	const float RefreshInterval = SCE_REMOTEPLAY_PROHIBIT_INTERVAL * 0.001f * 0.001f * 0.8f;

	void* heap = nullptr;
	bool isEnabled = false;
	float secondsSinceProhibit = 0.0f;
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

	isEnabled = true;
	return RemotePlayResult::Ok;
}

RemotePlayResult RemotePlay::Terminate()
{
	isEnabled = false;

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

RemotePlayResult RemotePlay::Update(float elapsedSeconds)
{
	assert(heap != nullptr);

	if (isEnabled)
		return RemotePlayResult::Ok;

	// Renew remote play prohibition at regular intervals
	secondsSinceProhibit += elapsedSeconds;
	if (secondsSinceProhibit >= RefreshInterval) {
		secondsSinceProhibit = 0;
		auto ret = sceRemoteplayProhibit();
		if (ret != SCE_OK) {
			return (RemotePlayResult)ret;
		}
	}

	return RemotePlayResult::Ok;
}

bool RemotePlay::GetIsEnabled()
{
	return isEnabled;
}

void RemotePlay::SetIsEnabled(bool value)
{
	if (isEnabled == value)
		return;

	// If we were disabled and we're now enabling,
	// let the system know immediately.
	if (!isEnabled && value) {
		auto ret = sceRemoteplayApprove();
		assert(ret == SCE_OK);
	}

	// If we're disabling remote play, update
	// immediately and schedule repeats.
	if (!value)
	{
		auto ret = sceRemoteplayProhibit();
		assert(ret == SCE_OK);
		secondsSinceProhibit = 0;
	}

	isEnabled = value;
}
