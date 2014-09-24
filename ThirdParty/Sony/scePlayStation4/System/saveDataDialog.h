#pragma once

#include "predecls.h"
#include "commonDialog.h"

#include <save_data_dialog.h>
#include <sceerror.h>

namespace System {

enum class SaveDataDialogSysMsg
{
	Invalid = SCE_SAVE_DATA_DIALOG_SYSMSG_TYPE_INVALID,
	NoData = SCE_SAVE_DATA_DIALOG_SYSMSG_TYPE_NODATA,
	Confirm = SCE_SAVE_DATA_DIALOG_SYSMSG_TYPE_CONFIRM,
	Overwrite = SCE_SAVE_DATA_DIALOG_SYSMSG_TYPE_OVERWRITE,
	NoSpace = SCE_SAVE_DATA_DIALOG_SYSMSG_TYPE_NOSPACE,
	Progress = SCE_SAVE_DATA_DIALOG_SYSMSG_TYPE_PROGRESS,
	FileCorrupted = SCE_SAVE_DATA_DIALOG_SYSMSG_TYPE_FILE_CORRUPTED,
	Finished = SCE_SAVE_DATA_DIALOG_SYSMSG_TYPE_FINISHED,
	NoSpaceContinuable = SCE_SAVE_DATA_DIALOG_SYSMSG_TYPE_NOSPACE_CONTINUABLE,
};

enum class SaveDataDialogType
{
	Invalid = SCE_SAVE_DATA_DIALOG_TYPE_INVALID,
	Save = SCE_SAVE_DATA_DIALOG_TYPE_SAVE,			
	Load = SCE_SAVE_DATA_DIALOG_TYPE_LOAD,
	Delete = SCE_SAVE_DATA_DIALOG_TYPE_DELETE,
};


class CS_API SaveDataDialog
{	
private:
	 SceSaveDataDialogParam  _openParam;
	 SceSaveDataDialogCloseParam _closeParam;
	 SceSaveDataDialogItems _items;
	 SceSaveDataTitleId _titleId;
	 SceSaveDataDirName _dirName;

public:

	SaveDataDialog();
	~SaveDataDialog();

	CommonDialogError OpenSystemMsg(SaveDataDialogSysMsg message, uint64_t value);

	CommonDialogError ForceClose();

	CommonDialogStatus UpdateStatus();

	void SetUserId(SceUserServiceUserId userId);
	SceUserServiceUserId GetUserId();

	void SetTitleId(const char* titleId);
	const char* GetTitleId();

	void SetDirectoryName(const char* dirName);
	const char* GetDirectoryName();

	void SetDisplayType(SaveDataDialogType type);
	SaveDataDialogType GetDisplayType();
};

} // namespace System
