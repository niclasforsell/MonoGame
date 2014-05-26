#include "saveData.h"

#include <memory.h>
#include <assert.h>

#include <save_data.h>
#include <scebase_common.h>


using namespace System;

bool SaveData::_initialized = false;


SaveDataResult SaveData::Initialize(ThreadPrio priority)
{
	SceSaveDataInitParams params;
	memset(&params, 0, sizeof(params));
	params.priority = priority;

	auto result = sceSaveDataInitialize(&params);
	if (result == SCE_OK)
		_initialized = true;

	return (SaveDataResult)result;
}

SaveDataResult SaveData::Terminate()
{
	auto result = sceSaveDataTerminate();
	if (result == SCE_OK)
		_initialized = false;

	return (SaveDataResult)result;
}

SaveData::SaveData(UserServiceUserId userId, const char *titleId, const char *dirName, const char *fingerprint)
{
	if (!_initialized)
		Initialize(ThreadPrio::Default);

	_userId = userId;

	memset(&_titleId, 0, sizeof(_titleId));
	if (titleId != NULL)
		strncpy(_titleId.data, titleId, SCE_SAVE_DATA_TITLE_ID_DATA_SIZE);
	memset(&_dirName, 0, sizeof(_dirName));
	if (dirName != NULL)
		strncpy(_dirName.data, dirName, SCE_SAVE_DATA_DIRNAME_DATA_MAXSIZE);
	memset(&_fingerprint, 0, sizeof(_fingerprint));
	if (fingerprint != NULL)
		strncpy(_fingerprint.data, fingerprint, SCE_SAVE_DATA_FINGERPRINT_DATA_SIZE);

	memset(&_mountPoint, 0, sizeof(_mountPoint));
	memset(_title, 0, sizeof(_title));
	memset(_subTitle, 0, sizeof(_subTitle));
	memset(_detail, 0, sizeof(_detail));
}

SaveData::~SaveData()
{
	if (_mountPoint.data[0] != 0)
		Unmount();
}

SaveDataResult SaveData::Mount(	uint64_t blocks,
								SaveDataMountMode mountMode,
								CS_OUT uint64_t &requiredBlocks,
								CS_OUT uint32_t &progress)
{
	SceSaveDataMount mount;
	memset(&mount, 0, sizeof(mount));
	mount.userId = _userId;
	mount.titleId = _titleId.data[0] ? &_titleId : NULL;
	mount.dirName = _dirName.data[0] ? &_dirName : NULL;
	mount.fingerprint = _fingerprint.data[0] ? &_fingerprint : NULL;
	mount.blocks = blocks;
	mount.mountMode = mountMode;

	SceSaveDataMountResult mountResult;
	memset(&mountResult, 0, sizeof(mountResult));

	auto result = sceSaveDataMount(&mount, &mountResult);
	if (result >= SCE_OK)
	{
		memcpy(&_mountPoint, &mountResult.mountPoint, sizeof(_mountPoint));
		requiredBlocks = mountResult.requiredBlocks;
		progress = mountResult.progress;
	}

	return (SaveDataResult)result;
}

SaveDataResult SaveData::Unmount()
{
	auto result = sceSaveDataUmount(&_mountPoint);
	if (result == SCE_OK)
	{
		memset(&_mountPoint, 0, sizeof(_mountPoint));
		memset(_title, 0, sizeof(_title));
		memset(_subTitle, 0, sizeof(_subTitle));
		memset(_detail, 0, sizeof(_detail));
	}
	return (SaveDataResult)result;
}

SaveDataResult SaveData::GetMountInfo(	CS_OUT uint64_t *blocks,
										CS_OUT uint64_t *freeBlocks)
{
	SceSaveDataMountInfo info;
	memset(&info, 0, sizeof(info));

	auto result = sceSaveDataGetMountInfo(&_mountPoint, &info);
	if (result >= SCE_OK)
	{
		*blocks = info.blocks;
		*freeBlocks = info.freeBlocks;
	}

	return (SaveDataResult)result;
}

const char* SaveData::GetTitle()
{
	auto result = sceSaveDataGetParam(&_mountPoint, SCE_SAVE_DATA_PARAM_TYPE_TITLE, _title, SCE_SAVE_DATA_TITLE_MAXSIZE, NULL);
	assert(result == SCE_OK);
	return _title;
}

const char* SaveData::GetSubTitle()
{
	auto result = sceSaveDataGetParam(&_mountPoint, SCE_SAVE_DATA_PARAM_TYPE_SUB_TITLE, _subTitle, SCE_SAVE_DATA_SUBTITLE_MAXSIZE, NULL);
	assert(result == SCE_OK);
	return _subTitle;
}

const char* SaveData::GetDetail()
{
	auto result = sceSaveDataGetParam(&_mountPoint, SCE_SAVE_DATA_PARAM_TYPE_DETAIL, _detail, SCE_SAVE_DATA_DETAIL_MAXSIZE, NULL);
	assert(result == SCE_OK);
	return _detail;
}

uint32_t SaveData::GetUserParam()
{
	uint32_t userParam;
	auto result = sceSaveDataGetParam(&_mountPoint, SCE_SAVE_DATA_PARAM_TYPE_USER_PARAM, &userParam, sizeof(uint32_t), NULL);
	assert(result == SCE_OK);
	return userParam;
}

time_t SaveData::GetMTime()
{
	time_t mtime;
	auto result = sceSaveDataGetParam(&_mountPoint, SCE_SAVE_DATA_PARAM_TYPE_MTIME, &mtime, sizeof(time_t), NULL);
	assert(result == SCE_OK);
	return mtime;

}

void SaveData::SetTitle(const char *title)
{
	auto len = strlen(title);
	assert(len < SCE_SAVE_DATA_TITLE_MAXSIZE);
	auto result = sceSaveDataSetParam(&_mountPoint, SCE_SAVE_DATA_PARAM_TYPE_TITLE, title, len);
	assert(result == SCE_OK);
}

void SaveData::SetSubTitle(const char *subTitle)
{
	auto len = strlen(subTitle);
	assert(len < SCE_SAVE_DATA_SUBTITLE_MAXSIZE);
	auto result = sceSaveDataSetParam(&_mountPoint, SCE_SAVE_DATA_PARAM_TYPE_SUB_TITLE, subTitle, len);
	assert(result == SCE_OK);
}

void SaveData::SetDetail(const char *detail)
{
	auto len = strlen(detail);
	assert(len < SCE_SAVE_DATA_DETAIL_MAXSIZE);
	auto result = sceSaveDataSetParam(&_mountPoint, SCE_SAVE_DATA_PARAM_TYPE_DETAIL, detail, len);
	assert(result == SCE_OK);
}

void SaveData::SetUserParam(uint32_t userParam)
{
	auto result = sceSaveDataSetParam(&_mountPoint, SCE_SAVE_DATA_PARAM_TYPE_USER_PARAM, &userParam, sizeof(uint32_t));
	assert(result == SCE_OK);
}

void SaveData::SetMTime(time_t mtime)
{
	auto result = sceSaveDataSetParam(&_mountPoint, SCE_SAVE_DATA_PARAM_TYPE_MTIME, &mtime, sizeof(time_t));
	assert(result == SCE_OK);
}

SaveDataResult SaveData::LoadIcon(void *buffer, size_t bufferSize, CS_OUT size_t *dataSize)
{
	SceSaveDataIcon icon;
	memset(&icon, 0, sizeof(icon));
	icon.buf = buffer;
	icon.bufSize = bufferSize;

	auto result = sceSaveDataLoadIcon(&_mountPoint, &icon);
	if (result >= SCE_OK)
		*dataSize = icon.dataSize;

	return (SaveDataResult)result;
}

SaveDataResult SaveData::SaveIcon(void *buffer, size_t bufferSize, size_t dataSize)
{
	SceSaveDataIcon icon;
	memset(&icon, 0, sizeof(icon));
	icon.buf = buffer;
	icon.bufSize = bufferSize;
	icon.dataSize = dataSize;

	auto result = sceSaveDataSaveIcon(&_mountPoint, &icon);
	return (SaveDataResult)result;
}

SaveDataResult SaveData::Delete(	UserServiceUserId userId, 
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
