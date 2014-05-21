#include "saveData.h"

#include <memory.h>
#include <assert.h>

#include <save_data.h>
#include <scebase_common.h>


using namespace System;

SaveDataResult SaveData::Initialize(ThreadPrio priority)
{
	SceSaveDataInitParams params;
	memset(&params, 0, sizeof(params));
	params.priority = priority;

	auto result = sceSaveDataInitialize(&params);
	return (SaveDataResult)result;
}

SaveDataResult SaveData::Terminate()
{
	auto result = sceSaveDataTerminate();
	return (SaveDataResult)result;
}

SaveDataResult SaveData::Mount(	uint32_t userId, 
								const char *titleId, 
								const char *dirName, 
								const char *fingerprint,
								uint64_t blocks,
								SaveDataMountMode mountMode,
								CS_OUT char *mountPoint,
								CS_OUT uint64_t &requiredBlocks,
								CS_OUT uint32_t &progress)
{
	SceSaveDataTitleId title;
	if (titleId != NULL)
	{
		memset(&title, 0, sizeof(title));
		strncpy(title.data, titleId, SCE_SAVE_DATA_TITLE_ID_DATA_SIZE);
	}

	SceSaveDataDirName dir;
	if (dirName != NULL)
	{
		memset(&dir, 0, sizeof(dir));
		strncpy(dir.data, dirName, SCE_SAVE_DATA_DIRNAME_DATA_MAXSIZE);
	}

	SceSaveDataFingerprint print;
	if (fingerprint != NULL)
	{
		memset(&print, 0, sizeof(print));
		strncpy(print.data, fingerprint, SCE_SAVE_DATA_FINGERPRINT_DATA_SIZE);
	}

	SceSaveDataMount mount;
	memset(&mount, 0, sizeof(mount));
	mount.userId = userId;
	mount.titleId = titleId ? &title : NULL; 
	mount.dirName = dirName ? &dir : NULL;
	mount.fingerprint = fingerprint ? &print : NULL;
	mount.blocks = blocks;
	mount.mountMode = mountMode;

	SceSaveDataMountResult mountResult;
	memset(&mountResult, 0, sizeof(mountResult));

	auto result = sceSaveDataMount(&mount, &mountResult);
	if (result >= SCE_OK)
	{
		//*mountPoint = std::string(mountResult.mountPoint.data);
		strncpy(mountPoint, mountResult.mountPoint.data, SCE_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE);
		requiredBlocks = mountResult.requiredBlocks;
		progress = mountResult.progress;
	}

	return (SaveDataResult)result;
}

SaveDataResult SaveData::GetMountInfo(	const char *mountPoint,
										CS_OUT uint64_t *blocks,
										CS_OUT uint64_t *freeBlocks)
{
	SceSaveDataMountPoint mount;
	memset(&mount, 0, sizeof(mount));
	strncpy(mount.data, mountPoint, SCE_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE);

	SceSaveDataMountInfo info;
	memset(&info, 0, sizeof(info));

	auto result = sceSaveDataGetMountInfo(&mount, &info);
	if (result >= SCE_OK)
	{
		*blocks = info.blocks;
		*freeBlocks = info.freeBlocks;
	}

	return (SaveDataResult)result;
}

SaveDataResult SaveData::GetParamTitle(const char *mountPoint, CS_OUT char *title)
{
	SceSaveDataMountPoint mount;
	memset(&mount, 0, sizeof(mount));
	strncpy(mount.data, mountPoint, SCE_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE);

	auto result = sceSaveDataGetParam(&mount, SCE_SAVE_DATA_PARAM_TYPE_TITLE, title, SCE_SAVE_DATA_TITLE_MAXSIZE, NULL);
	return (SaveDataResult)result;
}

SaveDataResult SaveData::GetParamSubTitle(const char *mountPoint, CS_OUT char *subTitle)
{
	SceSaveDataMountPoint mount;
	memset(&mount, 0, sizeof(mount));
	strncpy(mount.data, mountPoint, SCE_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE);

	auto result = sceSaveDataGetParam(&mount, SCE_SAVE_DATA_PARAM_TYPE_SUB_TITLE, subTitle, SCE_SAVE_DATA_SUBTITLE_MAXSIZE, NULL);
	return (SaveDataResult)result;
}

SaveDataResult SaveData::GetParamDetail(const char *mountPoint, CS_OUT char *detail)
{
	SceSaveDataMountPoint mount;
	memset(&mount, 0, sizeof(mount));
	strncpy(mount.data, mountPoint, SCE_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE);

	auto result = sceSaveDataGetParam(&mount, SCE_SAVE_DATA_PARAM_TYPE_DETAIL, detail, SCE_SAVE_DATA_DETAIL_MAXSIZE, NULL);
	return (SaveDataResult)result;
}

SaveDataResult SaveData::GetParamUserParam(const char *mountPoint, CS_OUT uint32_t *userParam)
{
	SceSaveDataMountPoint mount;
	memset(&mount, 0, sizeof(mount));
	strncpy(mount.data, mountPoint, SCE_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE);

	auto result = sceSaveDataGetParam(&mount, SCE_SAVE_DATA_PARAM_TYPE_USER_PARAM, userParam, sizeof(uint32_t), NULL);
	return (SaveDataResult)result;
}

SaveDataResult SaveData::GetParamMTime(const char *mountPoint, CS_OUT time_t *mtime)
{
	SceSaveDataMountPoint mount;
	memset(&mount, 0, sizeof(mount));
	strncpy(mount.data, mountPoint, SCE_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE);

	auto result = sceSaveDataGetParam(&mount, SCE_SAVE_DATA_PARAM_TYPE_MTIME, mtime, sizeof(time_t), NULL);
	return (SaveDataResult)result;
}

SaveDataResult SaveData::SetParamTitle(const char *mountPoint, const char *title)
{
	SceSaveDataMountPoint mount;
	memset(&mount, 0, sizeof(mount));
	strncpy(mount.data, mountPoint, SCE_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE);

	auto result = sceSaveDataSetParam(&mount, SCE_SAVE_DATA_PARAM_TYPE_TITLE, title, SCE_SAVE_DATA_TITLE_MAXSIZE);
	return (SaveDataResult)result;
}

SaveDataResult SaveData::SetParamSubTitle(const char *mountPoint, const char *subTitle)
{
	SceSaveDataMountPoint mount;
	memset(&mount, 0, sizeof(mount));
	strncpy(mount.data, mountPoint, SCE_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE);

	auto result = sceSaveDataSetParam(&mount, SCE_SAVE_DATA_PARAM_TYPE_SUB_TITLE, subTitle, SCE_SAVE_DATA_SUBTITLE_MAXSIZE);
	return (SaveDataResult)result;
}

SaveDataResult SaveData::SetParamDetail(const char *mountPoint, const char *detail)
{
	SceSaveDataMountPoint mount;
	memset(&mount, 0, sizeof(mount));
	strncpy(mount.data, mountPoint, SCE_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE);

	auto result = sceSaveDataSetParam(&mount, SCE_SAVE_DATA_PARAM_TYPE_DETAIL, detail, SCE_SAVE_DATA_DETAIL_MAXSIZE);
	return (SaveDataResult)result;
}

SaveDataResult SaveData::SetParamUserParam(const char *mountPoint, uint32_t userParam)
{
	SceSaveDataMountPoint mount;
	memset(&mount, 0, sizeof(mount));
	strncpy(mount.data, mountPoint, SCE_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE);

	auto result = sceSaveDataSetParam(&mount, SCE_SAVE_DATA_PARAM_TYPE_USER_PARAM, &userParam, sizeof(uint32_t));
	return (SaveDataResult)result;
}

SaveDataResult SaveData::SetParamMTime(const char *mountPoint, time_t mtime)
{
	SceSaveDataMountPoint mount;
	memset(&mount, 0, sizeof(mount));
	strncpy(mount.data, mountPoint, SCE_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE);

	auto result = sceSaveDataSetParam(&mount, SCE_SAVE_DATA_PARAM_TYPE_MTIME, &mtime, sizeof(time_t));
	return (SaveDataResult)result;
}

SaveDataResult SaveData::LoadIcon(const char *mountPoint, void *buffer, size_t bufferSize, CS_OUT size_t *dataSize)
{
	SceSaveDataMountPoint mount;
	memset(&mount, 0, sizeof(mount));
	strncpy(mount.data, mountPoint, SCE_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE);

	SceSaveDataIcon icon;
	memset(&icon, 0, sizeof(icon));
	icon.buf = buffer;
	icon.bufSize = bufferSize;

	auto result = sceSaveDataLoadIcon(&mount, &icon);
	if (result >= SCE_OK)
		*dataSize = icon.dataSize;

	return (SaveDataResult)result;
}

SaveDataResult SaveData::SaveIcon(const char *mountPoint, void *buffer, size_t bufferSize, size_t dataSize)
{
	SceSaveDataMountPoint mount;
	memset(&mount, 0, sizeof(mount));
	strncpy(mount.data, mountPoint, SCE_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE);

	SceSaveDataIcon icon;
	memset(&icon, 0, sizeof(icon));
	icon.buf = buffer;
	icon.bufSize = bufferSize;
	icon.dataSize = dataSize;

	auto result = sceSaveDataSaveIcon(&mount, &icon);
	return (SaveDataResult)result;
}

SaveDataResult SaveData::Delete(	uint32_t userId, 
									const char *titleId, 
									const char *dirName, 
									CS_OUT uint32_t *progress)
{
	SceSaveDataTitleId title;
	if (titleId != NULL)
	{
		memset(&title, 0, sizeof(title));
		strncpy(title.data, titleId, SCE_SAVE_DATA_TITLE_ID_DATA_SIZE);
	}

	SceSaveDataDirName dir;
	if (dirName != NULL)
	{
		memset(&dir, 0, sizeof(dir));
		strncpy(dir.data, dirName, SCE_SAVE_DATA_DIRNAME_DATA_MAXSIZE);
	}

	SceSaveDataDelete delete_;
	memset(&delete_, 0, sizeof(delete_));
	delete_.userId = userId;
	delete_.titleId = titleId ? &title : NULL; 
	delete_.dirName = dirName ? &dir : NULL;

	auto result = sceSaveDataDelete(&delete_);
	if (result >= SCE_OK)
		*progress = delete_.progress;

	return (SaveDataResult)result;
}

SaveDataResult SaveData::Unmount(const char *mountPoint)
{
	SceSaveDataMountPoint mount;
	memset(&mount, 0, sizeof(mount));
	strncpy(mount.data, mountPoint, SCE_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE);

	auto result = sceSaveDataUmount(&mount);
	return (SaveDataResult)result;
}