#include "saveDataDialog.h"
#include <libsysmodule.h>
#include <assert.h>

using namespace System;


SaveDataDialog::SaveDataDialog()
{
	sceSaveDataDialogParamInitialize(&_openParam);
	memset(&_closeParam, 0, sizeof(_closeParam));

	auto error = sceSaveDataDialogInitialize();
	assert(error == SCE_OK);
}

SaveDataDialog::~SaveDataDialog()
{
	auto error = sceSaveDataDialogTerminate();
	assert(error == SCE_OK);
}

CommonDialogError SaveDataDialog::OpenSystemMsg(SaveDataDialogSysMsg message, uint64_t value)
{
	sceSaveDataDialogParamInitialize(&_openParam);
	_openParam.dispType = SCE_SAVE_DATA_DIALOG_TYPE_SAVE;
	_openParam.mode = SCE_SAVE_DATA_DIALOG_MODE_SYSTEM_MSG;

	SceSaveDataDialogSystemMessageParam systemMessageParam;
	memset(&systemMessageParam, 0, sizeof(systemMessageParam));
	systemMessageParam.sysMsgType = (SceSaveDataDialogSystemMessageType)message;
	systemMessageParam.value = value;
	_openParam.sysMsgParam = &systemMessageParam;

	auto error = sceSaveDataDialogOpen(&_openParam);

	return (CommonDialogError)error;
}

CommonDialogError SaveDataDialog::ForceClose()
{
	auto error = sceSaveDataDialogClose(&_closeParam);
	return (CommonDialogError)error;
}

/*
CommonDialogStatus SaveDataDialog::GetStatus()
{
	auto status = sceSaveDataDialogGetStatus();
	return (CommonDialogStatus)status;
}
*/

CommonDialogStatus SaveDataDialog::UpdateStatus()
{
	auto status = sceSaveDataDialogUpdateStatus();
	return (CommonDialogStatus)status;
}
