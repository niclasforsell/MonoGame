#pragma once
#include "predecls.h"

#include <sceerror.h>
#include <app_content.h>

namespace System {

enum class AppContentResult
{
	Ok = SCE_OK,

	ErrorNotInitialized = SCE_APP_CONTENT_ERROR_NOT_INITIALIZED,
	ErrorParameter = SCE_APP_CONTENT_ERROR_PARAMETER,
	ErrorBusy = SCE_APP_CONTENT_ERROR_BUSY,
	ErrorNotMounted = SCE_APP_CONTENT_ERROR_NOT_MOUNTED,
	ErrorNotFound = SCE_APP_CONTENT_ERROR_NOT_FOUND,
	ErrorMountFull = SCE_APP_CONTENT_ERROR_MOUNT_FULL,
	DrmNoEntitlement = SCE_APP_CONTENT_ERROR_DRM_NO_ENTITLEMENT,
	ErrorNoSpace = SCE_APP_CONTENT_ERROR_NO_SPACE,
	ErrorNotSupported = SCE_APP_CONTENT_ERROR_NOT_SUPPORTED,
	ErrorInternal = SCE_APP_CONTENT_ERROR_INTERNAL,
	ErrorDownloadEntryFull = SCE_APP_CONTENT_ERROR_DOWNLOAD_ENTRY_FULL,
	ErrorInvalidPkg = SCE_APP_CONTENT_ERROR_INVALID_PKG,
	ErrorOtherApplicationPkg = SCE_APP_CONTENT_ERROR_OTHER_APPLICATION_PKG,
	ErrorCreateFull = SCE_APP_CONTENT_ERROR_CREATE_FULL,
	ErrorMountOtherApp = SCE_APP_CONTENT_ERROR_MOUNT_OTHER_APP,
	ErrorOfMemory = SCE_APP_CONTENT_ERROR_OF_MEMORY,
};

enum class AppParamId
{
	SkuFlag = SCE_APP_CONTENT_APPPARAM_ID_SKU_FLAG,
	UserDefinedParam1 = SCE_APP_CONTENT_APPPARAM_ID_USER_DEFINED_PARAM_1,
	UserDefinedParam2 = SCE_APP_CONTENT_APPPARAM_ID_USER_DEFINED_PARAM_2,
	UserDefinedParam3 = SCE_APP_CONTENT_APPPARAM_ID_USER_DEFINED_PARAM_3,
	UserDefinedParam4 = SCE_APP_CONTENT_APPPARAM_ID_USER_DEFINED_PARAM_4,
};

enum class AppParamSkuFlag
{
	Trial = SCE_APP_CONTENT_APPPARAM_SKU_FLAG_TRIAL,
	Full = SCE_APP_CONTENT_APPPARAM_SKU_FLAG_FULL,
};

enum class TemporaryDataOption
{
	None = SCE_APP_CONTENT_TEMPORARY_DATA_OPTION_NONE,
	Format = SCE_APP_CONTENT_TEMPORARY_DATA_OPTION_FORMAT,	
};


class CS_API AppContent
{
	AppContent() {}

	static SceAppContentMountPoint _tempMount;

public:

	static AppContentResult Initialize(CS_OUT uint32_t &bootParam);
	
	static AppContentResult AppParamGetInt(AppParamId paramId, CS_OUT int32_t &value);

	static AppContentResult TemporaryDataMount(TemporaryDataOption option, CS_OUT const char *&mountPoint);
	static AppContentResult TemporaryDataFormat(const char *mountPoint);
	static AppContentResult TemporaryDataUnmount(const char *mountPoint);

};

} // namespace System
