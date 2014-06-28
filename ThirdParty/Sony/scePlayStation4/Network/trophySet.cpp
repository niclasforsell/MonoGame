#include "trophySet.h"
#include <assert.h>
#include <libsysmodule.h>

using namespace Network;

TrophySet::TrophySet()
	: _context(SCE_NP_TROPHY_INVALID_CONTEXT)
{
	if (sceSysmoduleIsLoaded(SCE_SYSMODULE_NP_TROPHY) != SCE_SYSMODULE_LOADED)
	{
		auto ret = sceSysmoduleLoadModule(SCE_SYSMODULE_NP_TROPHY);
		assert(ret == SCE_OK);
	}
}

TrophySet::~TrophySet()
{
	if (_context != SCE_NP_TROPHY_INVALID_CONTEXT)
	{
		auto ret = sceNpTrophyDestroyContext(_context);
		assert(ret == SCE_OK);
		_context = SCE_NP_TROPHY_INVALID_CONTEXT;
	}
}

TrophyResult TrophySet::CreateContext(const SceUserServiceUserId userId, const SceNpServiceLabel serviceLabel)
{
	assert(_context == SCE_NP_TROPHY_INVALID_CONTEXT);
	SceNpTrophyHandle handle = SCE_NP_TROPHY_INVALID_HANDLE;

	auto ret = sceNpTrophyCreateContext(&_context, userId, serviceLabel, 0);
	if (ret != SCE_OK)
		goto cleanup;

	ret = sceNpTrophyCreateHandle(&handle);
	if (ret != SCE_OK)
		goto cleanup;

	ret = sceNpTrophyRegisterContext(_context, handle, 0);
	if (ret != SCE_OK)
		goto cleanup;

cleanup:
	if (handle != SCE_NP_TROPHY_INVALID_HANDLE)
		sceNpTrophyDestroyHandle(handle);

	return (TrophyResult)ret;
}

TrophyResult TrophySet::IsUnlocked(int32_t trophyId, CS_OUT bool* isUnlocked)
{
	assert(_context != SCE_NP_TROPHY_INVALID_CONTEXT);
	assert(trophyId != SCE_NP_TROPHY_INVALID_TROPHY_ID);

	SceNpTrophyHandle handle = SCE_NP_TROPHY_INVALID_HANDLE;
	auto ret = sceNpTrophyCreateHandle(&handle);
	if (ret != SCE_OK)
		return (TrophyResult)ret;

	SceNpTrophyFlagArray flags;
	uint32_t count = 0;
	SCE_NP_TROPHY_FLAG_ZERO(&flags);
	ret = sceNpTrophyGetTrophyUnlockState(_context, handle, &flags, &count);
	if (ret != SCE_OK)
		return (TrophyResult)ret;

	*isUnlocked = SCE_NP_TROPHY_FLAG_ISSET(trophyId, &flags);

	sceNpTrophyDestroyHandle(handle);
	return (TrophyResult)ret;
}

TrophyResult TrophySet::Unlock(int32_t trophyId, CS_OUT bool* platinumReceived)
{
	assert(_context != SCE_NP_TROPHY_INVALID_CONTEXT);
	assert(trophyId != SCE_NP_TROPHY_INVALID_TROPHY_ID);

	SceNpTrophyHandle handle = SCE_NP_TROPHY_INVALID_HANDLE;
	auto ret = sceNpTrophyCreateHandle(&handle);
	if (ret != SCE_OK)
		return (TrophyResult)ret;

	SceNpTrophyId platinumId;
	ret = sceNpTrophyUnlockTrophy(_context, handle, trophyId, &platinumId);

	*platinumReceived = platinumId != SCE_NP_TROPHY_INVALID_TROPHY_ID;

	sceNpTrophyDestroyHandle(handle);
	return (TrophyResult)ret;
}

