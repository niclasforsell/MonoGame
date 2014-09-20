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


class CS_API SaveDataDialog
{	
private:
	 SceSaveDataDialogParam  _openParam;
	 SceSaveDataDialogCloseParam _closeParam;

public:

	SaveDataDialog();
	~SaveDataDialog();

	CommonDialogError OpenSystemMsg(SaveDataDialogSysMsg message, uint64_t value);

	CommonDialogError ForceClose();

	//CommonDialogStatus GetStatus();

	CommonDialogStatus UpdateStatus();
};

} // namespace System
