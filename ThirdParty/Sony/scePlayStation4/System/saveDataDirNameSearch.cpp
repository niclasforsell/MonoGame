#include "saveDataDirNameSearch.h"

#include "saveData.h"
#include "..\allocator.h"

#include <memory.h>
#include <assert.h>

#include <save_data.h>
#include <scebase_common.h>


using namespace System;

SaveDataDirNameSearch::SaveDataDirNameSearch(SceUserServiceUserId userId, int maxResults)
{
	memset(&_titleId, 0, sizeof(_titleId));
	memset(&_dirName, 0, sizeof(_dirName));
	memset(&_results, 0, sizeof(_results));

	_maxResults = MIN(SCE_SAVE_DATA_DIRNAME_MAX_COUNT, maxResults);
	_names = (SceSaveDataDirName*)mem::alloc(sizeof(SceSaveDataDirName) * _maxResults);
	_params = (SceSaveDataParam*)mem::alloc(sizeof(SceSaveDataParam) * _maxResults);

	memset(&_cond, 0, sizeof(_cond));
	_cond.userId = userId;
}

SaveDataDirNameSearch::~SaveDataDirNameSearch()
{
	mem::free(_names);
	mem::free(_params);
}

SaveDataResult SaveDataDirNameSearch::Search(const char *dirNameOrNull)
{
	if (!dirNameOrNull || !dirNameOrNull[0])
		_cond.dirName = nullptr;
	else
	{
		auto len = MIN(strlen(dirNameOrNull), SCE_SAVE_DATA_DIRNAME_DATA_MAXSIZE);	
		strncpy(_dirName.data, dirNameOrNull, len);	
		_cond.dirName = &_dirName;
	}

	if (_titleId.data[0])
		_cond.titleId = &_titleId;
	else
		_cond.titleId = nullptr;

	// Clear previous results.
	memset(&_results, 0, sizeof(_results));	
	_results.dirNames = _names;
	_results.dirNamesNum = _maxResults;
	_results.params = _params;

	// Do search.
	auto err = sceSaveDataDirNameSearch(&_cond, &_results);
	return (SaveDataResult)err;
}

int SaveDataDirNameSearch::GetHits()
{
	return _results.hitNum;
}

const char* SaveDataDirNameSearch::GetDirName(int index)
{
	if (index < 0 || index >= _results.hitNum)
		return nullptr;

	return _names[index].data;
}

const char* SaveDataDirNameSearch::GetTitle(int index)
{
	if (index < 0 || index >= _results.hitNum)
		return nullptr;

	return _params[index].title;
}

const char* SaveDataDirNameSearch::GetSubTitle(int index)
{
	if (index < 0 || index >= _results.hitNum)
		return nullptr;

	return _params[index].subTitle;
}

const char* SaveDataDirNameSearch::GetDetail(int index)
{
	if (index < 0 || index >= _results.hitNum)
		return nullptr;

	return _params[index].detail;
}

uint32_t SaveDataDirNameSearch::GetUserParam(int index)
{
	if (index < 0 || index >= _results.hitNum)
		return 0;

	return _params[index].userParam;
}

time_t SaveDataDirNameSearch::GetModifiedTime(int index)
{
	if (index < 0 || index >= _results.hitNum)
		return 0;

	return _params[index].mtime;
}

void SaveDataDirNameSearch::SetSortKey(SaveDataSortKey key)
{
	_cond.key = (SceSaveDataSortKey)key;
}

SaveDataSortKey SaveDataDirNameSearch::GetSortKey()
{
	return (SaveDataSortKey)_cond.key;
}

void SaveDataDirNameSearch::SetSortOrder(SaveDataSortOrder order)
{
	_cond.order = (SceSaveDataSortOrder)order;
}

SaveDataSortOrder SaveDataDirNameSearch::GetSortOrder()
{
	return (SaveDataSortOrder)_cond.order;
}

void SaveDataDirNameSearch::SetTitleId(const char* titleId)
{
	auto len = MIN(strlen(titleId), SCE_SAVE_DATA_TITLE_ID_DATA_SIZE);	
	strncpy(_titleId.data, titleId, len);	
}

const char* SaveDataDirNameSearch::GetTitleId()
{
	return _titleId.data;
}

