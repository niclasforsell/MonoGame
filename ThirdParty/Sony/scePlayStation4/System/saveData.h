#pragma once

#include "predecls.h"
#include "threadPrio.h"

#include <string>

#include <save_data.h>
#include <save_data/save_data_error.h>
#include <sceerror.h>

namespace System {

enum class SaveDataResult
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

CS_FLAGS enum class SaveDataMountMode
{
	ReadOnly = SCE_SAVE_DATA_MOUNT_MODE_RDONLY,
	WriteOnly = SCE_SAVE_DATA_MOUNT_MODE_WRONLY,
	Create = SCE_SAVE_DATA_MOUNT_MODE_CREATE,
	DestructOff = SCE_SAVE_DATA_MOUNT_MODE_DESTRUCT_OFF,
#if SCE_ORBIS_SDK_VERSION >= 0x01700081u // SDK Version 1.7
	CopyIcon = SCE_SAVE_DATA_MOUNT_MODE_COPY_ICON,
#endif
};

CS_FLAGS enum class SaveDataConst : uint
{
	BlockSize = SCE_SAVE_DATA_BLOCK_SIZE,
	BlocksMin = SCE_SAVE_DATA_BLOCKS_MIN,
	IconWidth = SCE_SAVE_DATA_ICON_WIDTH,
	IconHeight = SCE_SAVE_DATA_ICON_HEIGHT,
};


class CS_API SaveData
{
	SaveData() { }

	static bool _initialized;

	UserServiceUserId _userId;

	SceSaveDataTitleId _titleId;
	SceSaveDataDirName _dirName;
	SceSaveDataFingerprint _fingerprint;

	char _title[SCE_SAVE_DATA_TITLE_MAXSIZE];
	char _subTitle[SCE_SAVE_DATA_PARAM_TYPE_SUB_TITLE];
	char _detail[SCE_SAVE_DATA_DETAIL_MAXSIZE];

	SceSaveDataMountPoint _mountPoint;

public:
		
	static SaveDataResult Initialize(ThreadPrio priority);

	static SaveDataResult Terminate();

	static SaveDataResult Delete(	UserServiceUserId userId, 
									const char *titleId, 
									const char *dirName, 
									CS_OUT uint32_t *progress);

	SaveData(UserServiceUserId userId, const char *titleId, const char *dirName, const char *fingerprint);

	~SaveData();

	SaveDataResult Mount(	uint64_t blocks,
							SaveDataMountMode mountMode,
							CS_OUT uint64_t &requiredBlocks,
							CS_OUT uint32_t &progress);

	SaveDataResult Unmount();

	const char* GetMountPoint() const;

	const char* GetTitle();
	void		SetTitle(const char *title);

	const char* GetSubTitle();
	void		SetSubTitle(const char *subTitle);

	const char* GetDetail();
	void		SetDetail(const char *detail);

	uint32_t	GetUserParam();
	void		SetUserParam(uint32_t userParam);

	time_t	GetMTime();
	void	SetMTime(time_t userParam);

	SaveDataResult GetMountInfo(	CS_OUT uint64_t *blocks,
									CS_OUT uint64_t *freeBlocks);

	SaveDataResult LoadIcon(void *buffer, size_t bufferSize, CS_OUT size_t *dataSize);
	SaveDataResult SaveIcon(void *buffer, size_t bufferSize, size_t dataSize);

};

} // namespace System
