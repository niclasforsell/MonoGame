#include "msgDialog.h"
#include <libsysmodule.h>
#include <assert.h>

using namespace System;


CommonDialogError MsgDialog::Initialize()
{
	if (sceSysmoduleIsLoaded(SCE_SYSMODULE_MESSAGE_DIALOG) != SCE_SYSMODULE_LOADED)
	{
		auto error = sceSysmoduleLoadModule(SCE_SYSMODULE_MESSAGE_DIALOG);
		assert(error == SCE_OK);
	}

	auto error = sceMsgDialogInitialize();
	return (CommonDialogError)error;
}

CommonDialogError MsgDialog::Terminate()
{
	auto error = sceMsgDialogTerminate();
	return (CommonDialogError)error;
}

CommonDialogError OpenUserMsg(const char *message, MsgDialogButtonType buttonType, UserServiceUserId userId)
{
	SceMsgDialogParam param;
	sceMsgDialogParamInitialize(&param);
	param.userId = userId;

	SceMsgDialogUserMessageParam userMsgParam;
	memset(&userMsgParam, 0, sizeof(userMsgParam));
	userMsgParam.buttonType = (SceMsgDialogButtonType)buttonType;
	userMsgParam.msg = message;
	param.userMsgParam = &userMsgParam;

	auto error = sceMsgDialogOpen(&param);
	return (CommonDialogError)error;
}

CommonDialogError OpenProgressBar(const char *message, UserServiceUserId userId)
{
	SceMsgDialogParam param;
	sceMsgDialogParamInitialize(&param);
	param.userId = userId;

	SceMsgDialogProgressBarParam progBarParam;
	memset(&progBarParam, 0, sizeof(progBarParam));
	progBarParam.barType = SCE_MSG_DIALOG_PROGRESSBAR_TYPE_PERCENTAGE;
	progBarParam.msg = message;
	param.progBarParam = &progBarParam;

	auto error = sceMsgDialogOpen(&param);
	return (CommonDialogError)error;
}

CommonDialogError OpenSystemMsg(MsgDialogSysMsgType sysMsgType, UserServiceUserId userId)
{
	SceMsgDialogParam param;
	sceMsgDialogParamInitialize(&param);
	param.userId = userId;

	SceMsgDialogSystemMessageParam sysMsgParam;
	memset(&sysMsgParam, 0, sizeof(sysMsgParam));
	sysMsgParam.sysMsgType = (SceMsgDialogSystemMessageType)sysMsgType;
	param.sysMsgParam = &sysMsgParam;

	auto error = sceMsgDialogOpen(&param);
	return (CommonDialogError)error;
}

CommonDialogError MsgDialog::Close()
{
	auto error = sceMsgDialogClose();
	return (CommonDialogError)error;
}

CommonDialogError MsgDialog::ProgressBarInc(uint32_t delta)
{
	auto error = sceMsgDialogProgressBarInc(SCE_MSG_DIALOG_PROGRESSBAR_TARGET_BAR_DEFAULT, delta);
	return (CommonDialogError)error;
}

CommonDialogError MsgDialog::ProgressBarSetMsg(const char* message)
{
	auto error = sceMsgDialogProgressBarSetMsg(SCE_MSG_DIALOG_PROGRESSBAR_TARGET_BAR_DEFAULT, message);
	return (CommonDialogError)error;
}

CommonDialogError MsgDialog::ProgressBarSetValue(uint32_t rate)
{
	auto error = sceMsgDialogProgressBarSetValue(SCE_MSG_DIALOG_PROGRESSBAR_TARGET_BAR_DEFAULT, rate);
	return (CommonDialogError)error;
}

CommonDialogStatus MsgDialog::GetStatus()
{
	auto status = sceMsgDialogGetStatus();
	return (CommonDialogStatus)status;
}

CommonDialogStatus MsgDialog::UpdateStatus()
{
	auto status = sceMsgDialogUpdateStatus();
	return (CommonDialogStatus)status;
}