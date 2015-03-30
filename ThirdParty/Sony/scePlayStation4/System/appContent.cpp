#include "appContent.h"

#include <assert.h>
#include <memory>
#include <libsysmodule.h>

using namespace System;


SceAppContentMountPoint AppContent::_tempMount;

AppContentResult AppContent::Initialize(CS_OUT uint32_t &bootParam)
{
	auto error = sceSysmoduleLoadModule(SCE_SYSMODULE_APP_CONTENT);
	assert(error == SCE_OK);

	SceAppContentInitParam initParam;
	memset(&initParam, 0, sizeof(initParam));
	SceAppContentBootParam bootParam_;
	memset(&bootParam_, 0, sizeof(bootParam_));

	auto result = sceAppContentInitialize(&initParam, &bootParam_);
	bootParam = bootParam_.attr;

	return (AppContentResult)result;
}

AppContentResult AppContent::AppParamGetInt(AppParamId paramId, CS_OUT int32_t &value)
{
	auto result = sceAppContentAppParamGetInt((SceAppContentAppParamId)paramId, &value);
	return (AppContentResult)result;
}

AppContentResult AppContent::TemporaryDataMount(TemporaryDataOption option, CS_OUT const char *&mountPoint)
{
	memset(&_tempMount, 0, sizeof(_tempMount));
	auto result = sceAppContentTemporaryDataMount2((SceAppContentTemporaryDataOption)option, &_tempMount);
	mountPoint = _tempMount.data;
	return (AppContentResult)result;
}

AppContentResult AppContent::TemporaryDataFormat(const char *mountPoint)
{
	SceAppContentMountPoint tempMount;
	memset(&tempMount, 0, sizeof(tempMount));
	auto len = MIN(strlen(mountPoint), SCE_APP_CONTENT_MOUNTPOINT_DATA_MAXSIZE-1);
	memcpy(tempMount.data, mountPoint, len);

	auto result = sceAppContentTemporaryDataFormat(&tempMount);
	return (AppContentResult)result;
}

AppContentResult AppContent::TemporaryDataUnmount(const char *mountPoint)
{
	SceAppContentMountPoint tempMount;
	memset(&tempMount, 0, sizeof(tempMount));
	auto len = MIN(strlen(mountPoint), SCE_APP_CONTENT_MOUNTPOINT_DATA_MAXSIZE-1);
	memcpy(tempMount.data, mountPoint, len);

	auto result = sceAppContentTemporaryDataUnmount(&tempMount);
	return (AppContentResult)result;
}
