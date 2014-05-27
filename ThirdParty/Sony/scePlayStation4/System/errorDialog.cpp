#include "errorDialog.h"
#include <libsysmodule.h>
#include <assert.h>

using namespace System;


ErrorDialogError ErrorDialog::Initialize()
{
	if (sceSysmoduleIsLoaded(SCE_SYSMODULE_ERROR_DIALOG) != SCE_SYSMODULE_LOADED)
	{
		auto error = sceSysmoduleLoadModule(SCE_SYSMODULE_ERROR_DIALOG);
		assert(error == SCE_OK);
	}

	auto error = sceErrorDialogInitialize();
	return (ErrorDialogError)error;
}

ErrorDialogError ErrorDialog::Terminate()
{
	auto error = sceErrorDialogTerminate();
	return (ErrorDialogError)error;
}

ErrorDialogError ErrorDialog::Open(int32_t errorCode, UserServiceUserId userId)
{
	SceErrorDialogParam param;
	sceErrorDialogParamInitialize(&param);

	param.errorCode = errorCode;
	param.userId = userId;

	auto error = sceErrorDialogOpen(&param);
	return (ErrorDialogError)error;
}

ErrorDialogError ErrorDialog::Close()
{
	auto error = sceErrorDialogClose();
	return (ErrorDialogError)error;
}

ErrorDialogStatus ErrorDialog::GetStatus()
{
	auto status = sceErrorDialogGetStatus();
	return (ErrorDialogStatus)status;
}

ErrorDialogStatus ErrorDialog::UpdateStatus()
{
	auto status = sceErrorDialogUpdateStatus();
	return (ErrorDialogStatus)status;
}