#pragma once

#include "predecls.h"
#include "threadPrio.h"

#include <string>

#include <save_data.h>
#include <save_data/save_data_error.h>
#include <sceerror.h>

namespace System {

enum SaveDataResult
{
	Ok = SCE_OK,

	ErrorParameter = SCE_SAVE_DATA_ERROR_PARAMETER,
	ErrorNotInitialized = SCE_SAVE_DATA_ERROR_NOT_INITIALIZED,
	ErrorOutOfMemory = SCE_SAVE_DATA_ERROR_OUT_OF_MEMORY,
	ErrorBusy = SCE_SAVE_DATA_ERROR_BUSY,
	ErrorNotMounted = SCE_SAVE_DATA_ERROR_NOT_MOUNTED,
	ErrorNoPermission = SCE_SAVE_DATA_ERROR_NO_PERMISSION,
	ErrroFingerprintMismatch = SCE_SAVE_DATA_ERROR_FINGERPRINT_MISMATCH,
	ErrorExists = SCE_SAVE_DATA_ERROR_EXISTS,
	ErrorNotFound = SCE_SAVE_DATA_ERROR_NOT_FOUND,
	ErrorNoSpace = SCE_SAVE_DATA_ERROR_NO_SPACE,
	ErrorNoSpaceFs = SCE_SAVE_DATA_ERROR_NO_SPACE_FS,
	ErrorInternal = SCE_SAVE_DATA_ERROR_INTERNAL,
	ErrorMountFull = SCE_SAVE_DATA_ERROR_MOUNT_FULL,
	ErrorBadMounted = SCE_SAVE_DATA_ERROR_BAD_MOUNTED,
	ErrorFileNotFound = SCE_SAVE_DATA_ERROR_FILE_NOT_FOUND,
	ErrorBroken = SCE_SAVE_DATA_ERROR_BROKEN,
	ErrorMountInhibit = SCE_SAVE_DATA_ERROR_MOUNT_INHIBIT,
	ErrorInvalidLoginUser = SCE_SAVE_DATA_ERROR_INVALID_LOGIN_USER,
	
};

CS_FLAGS enum SaveDataMountMode
{
	ReadOnly = SCE_SAVE_DATA_MOUNT_MODE_RDONLY,
	WriteOnly = SCE_SAVE_DATA_MOUNT_MODE_WRONLY,
	Create = SCE_SAVE_DATA_MOUNT_MODE_CREATE,
	DestructOff = SCE_SAVE_DATA_MOUNT_MODE_DESTRUCT_OFF,
};

class CS_API SaveData
{
	SaveData() { }

public:

	static SaveDataResult Initialize(ThreadPrio priority);

	static SaveDataResult Terminate();

	static SaveDataResult Mount(	uint32_t userId, 
									const char *titleId, 
									const char *dirName, 
									const char *fingerprint,
									uint64_t blocks,
									SaveDataMountMode mountMode,
									CS_OUT char *mountPoint,
									CS_OUT uint64_t &requiredBlocks,
									CS_OUT uint32_t &progress);

	static SaveDataResult GetMountInfo(	const char *mountPoint,
										CS_OUT uint64_t *blocks,
										CS_OUT uint64_t *freeBlocks);

	static SaveDataResult GetParamTitle(const char *mountPoint, CS_OUT char *title);
	static SaveDataResult GetParamSubTitle(const char *mountPoint, CS_OUT char *subTitle);
	static SaveDataResult GetParamDetail(const char *mountPoint, CS_OUT char *detail);
	static SaveDataResult GetParamUserParam(const char *mountPoint, CS_OUT uint32_t *userParam);
	static SaveDataResult GetParamMTime(const char *mountPoint, CS_OUT time_t *mtime);

	static SaveDataResult SetParamTitle(const char *mountPoint, const char *title);
	static SaveDataResult SetParamSubTitle(const char *mountPoint, const char *subTitle);
	static SaveDataResult SetParamDetail(const char *mountPoint, const char *detail);
	static SaveDataResult SetParamUserParam(const char *mountPoint, uint32_t userParam);
	static SaveDataResult SetParamMTime(const char *mountPoint, time_t mtime);

	static SaveDataResult LoadIcon(const char *mountPoint, void *buffer, size_t bufferSize, CS_OUT size_t *dataSize);
	static SaveDataResult SaveIcon(const char *mountPoint, void *buffer, size_t bufferSize, size_t dataSize);

	static SaveDataResult Delete(	uint32_t userId, 
									const char *titleId, 
									const char *dirName, 
									CS_OUT uint32_t *progress);

	static SaveDataResult Unmount(const char *mountPoint);

};

} // namespace System
