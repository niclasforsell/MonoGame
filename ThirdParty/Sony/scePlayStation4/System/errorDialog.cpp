#include "errorDialog.h"


using namespace System;


ErrorDialogError ErrorDialog::Initialize()
{
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