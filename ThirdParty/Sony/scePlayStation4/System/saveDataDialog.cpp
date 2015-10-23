#include "saveDataDialog.h"
#include <libsysmodule.h>
#include <assert.h>
#include <sys/param.h>

using namespace System;


SaveDataDialog::SaveDataDialog()
{
	sceSaveDataDialogParamInitialize(&_openParam);

	memset(&_closeParam, 0, sizeof(_closeParam));
	memset(&_items, 0, sizeof(_items));
	memset(&_titleId, 0, sizeof(_titleId));
	memset(&_dirName, 0, sizeof(_dirName));

	_items.dirName = &_dirName;

	_openParam.items = &_items;
	_openParam.dispType = SCE_SAVE_DATA_DIALOG_TYPE_SAVE;

	sceSysmoduleLoadModule(SCE_SYSMODULE_SAVE_DATA_DIALOG);
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

CommonDialogStatus SaveDataDialog::UpdateStatus()
{
	auto status = sceSaveDataDialogUpdateStatus();
	return (CommonDialogStatus)status;
}

CommonDialogError SaveDataDialog::GetResult(CS_OUT CommonDialogResult *result, CS_OUT SaveDataDialogButtonId *buttonId)
{
	SceSaveDataDialogResult sdr;
	memset(&sdr, 0, sizeof(sdr));
	auto error = sceSaveDataDialogGetResult(&sdr);

	*result = (CommonDialogResult)sdr.result;
	*buttonId = (SaveDataDialogButtonId)sdr.buttonId;

	return (CommonDialogError)error;
}

void SaveDataDialog::SetUserId(SceUserServiceUserId userId)
{
	_items.userId = userId;
}

SceUserServiceUserId SaveDataDialog::GetUserId()
{
	return _items.userId;
}

void SaveDataDialog::SetTitleId(const char* titleId)
{
	if (titleId == nullptr)
		_items.titleId = nullptr;
	else
	{
		auto len = MIN(strlen(titleId), SCE_SAVE_DATA_TITLE_ID_DATA_SIZE-1);
		memcpy(_titleId.data, titleId, len);
		_items.titleId = &_titleId;
	}
}

const char* SaveDataDialog::GetTitleId()
{
	return _titleId.data;
}

void SaveDataDialog::SetDirectoryName(const char* dirName)
{
	auto len = MIN(strlen(dirName), SCE_SAVE_DATA_DIRNAME_DATA_MAXSIZE);
	strncpy(_dirName.data, dirName, len);
	_items.dirNameNum = dirName == nullptr ? 0 : 1;
}

const char* SaveDataDialog::GetDirectoryName()
{
	return _dirName.data;
}

void SaveDataDialog::SetDisplayType(SaveDataDialogType type)
{
	_openParam.dispType = (SceSaveDataDialogType)type;
}

SaveDataDialogType SaveDataDialog::GetDisplayType()
{
	return (SaveDataDialogType)_openParam.dispType;
}
