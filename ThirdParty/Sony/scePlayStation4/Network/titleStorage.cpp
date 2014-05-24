#include "titleStorage.h"
#include <assert.h>
#include <libsysmodule.h>
#include <memory>

using namespace Network;

TitleStorage::TitleStorage()
	: _titleCtxId(-1)
{
	if (sceSysmoduleIsLoaded(SCE_SYSMODULE_NP_TUS) != SCE_SYSMODULE_LOADED)
	{
		auto ret = sceSysmoduleLoadModule(SCE_SYSMODULE_NP_TUS);
		assert(ret == SCE_OK);
	}
}

TitleStorage::~TitleStorage()
{
	if (_titleCtxId != -1)
	{
		auto ret = sceNpTssDeleteNpTitleCtx(_titleCtxId);
		assert(ret >= 0);
		_titleCtxId = -1;
	}
}

TitleStorageResult TitleStorage::CreateContext(const SceUserServiceUserId userId, const SceNpServiceLabel serviceLabel)
{
	SceNpId npId;
	auto ret = sceNpGetNpId(userId, &npId);
	if (ret < 0)
		return (TitleStorageResult)ret;

	ret = sceNpTssCreateNpTitleCtx(serviceLabel, &npId);
	if (ret < 0)
		return (TitleStorageResult)ret;

	ret = sceNpTssCreateNpTitleCtx(serviceLabel, &npId);
	if (ret < 0)
		return (TitleStorageResult)ret;

	_titleCtxId = ret;
	return TitleStorageResult::Ok;
}

TitleStorageResult TitleStorage::GetData(SceNpTssSlotId slotId, void* buffer, size_t bufferSize, CS_OUT size_t* recvdSize)
{
	assert(_titleCtxId != -1);
	assert(slotId < 16);
	assert(buffer != nullptr);
	assert(bufferSize <= (4 * 1024 * 1024));

	auto ret = 0;

	auto reqId = sceNpTusCreateRequest(_titleCtxId);
	if (reqId < 0)
		return (TitleStorageResult)reqId;

	SceNpTssDataStatus dataStatus;
	memset(&dataStatus, 0, sizeof(SceNpTssDataStatus));
	memset(buffer, 0, bufferSize);
	ret = sceNpTssGetData(reqId, slotId, &dataStatus, sizeof(SceNpTssDataStatus), buffer, bufferSize, NULL);
	if (ret >= 0)
		*recvdSize = ret;

	if (reqId > 0)
		sceNpTusDeleteRequest(reqId);

	return (TitleStorageResult)ret;
}

