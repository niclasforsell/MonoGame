#pragma once

#include "predecls.h"

#include <string>

#include <scebase.h>
#include <save_data.h>

namespace System {

enum class SaveDataResult;


enum class SaveDataSortKey  
{
	DirName = SCE_SAVE_DATA_SORT_KEY_DIRNAME,
	UserParam = SCE_SAVE_DATA_SORT_KEY_USER_PARAM,
	SizeKib = SCE_SAVE_DATA_SORT_KEY_SIZE_KIB,
	MTime = SCE_SAVE_DATA_SORT_KEY_MTIME,
};

enum class SaveDataSortOrder  
{
	Ascent = SCE_SAVE_DATA_SORT_ORDER_ASCENT,
	Descent = SCE_SAVE_DATA_SORT_ORDER_DESCENT,
};


class CS_API SaveDataDirNameSearch
{
private:

	int _maxResults;

	SceSaveDataTitleId _titleId;
	SceSaveDataDirName _dirName;
	SceSaveDataDirName *_names;
	SceSaveDataParam *_params;
	SceSaveDataDirNameSearchCond  _cond;
	SceSaveDataDirNameSearchResult _results;

public:

	SaveDataDirNameSearch(SceUserServiceUserId userId, int maxResults);
	~SaveDataDirNameSearch();

	SaveDataResult Search(const char *dirNameOrNull);

	int GetHits();

	const char* GetDirName(int index);
	const char* GetTitle(int index);
	const char* GetSubTitle(int index);
	const char* GetDetail(int index);
	uint32_t GetUserParam(int index);
	time_t GetModifiedTime(int index);

	void SetSortKey(SaveDataSortKey key);
	SaveDataSortKey GetSortKey();

	void SetSortOrder(SaveDataSortOrder order);
	SaveDataSortOrder GetSortOrder();

	void SetTitleId(const char *titleId);
	const char* GetTitleId();

};

} // namespace System
