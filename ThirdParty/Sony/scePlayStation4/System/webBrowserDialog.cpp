#include "webBrowserDialog.h"
#include <libsysmodule.h>
#include <assert.h>

using namespace System;


CommonDialogError WebBrowserDialog::Initialize()
{
	if (sceSysmoduleIsLoaded(SCE_SYSMODULE_WEB_BROWSER_DIALOG) != SCE_SYSMODULE_LOADED)
	{
		auto error = sceSysmoduleLoadModule(SCE_SYSMODULE_WEB_BROWSER_DIALOG);
		assert(error == SCE_OK);
	}

	auto error = sceWebBrowserDialogInitialize();
	return (CommonDialogError)error;
}

CommonDialogError WebBrowserDialog::Terminate()
{
	auto error = sceWebBrowserDialogTerminate();
	return (CommonDialogError)error;
}

CommonDialogError WebBrowserDialog::Open(const char *url, UserServiceUserId userId)
{
	SceWebBrowserDialogParam param;
	sceWebBrowserDialogParamInitialize(&param);

	param.url = url;
	param.mode = SCE_WEB_BROWSER_DIALOG_MODE_DEFAULT;
	param.userId = userId;

	auto error = sceWebBrowserDialogOpen(&param);
	return (CommonDialogError)error;
}

CommonDialogError WebBrowserDialog::Close()
{
	auto error = sceWebBrowserDialogClose();
	return (CommonDialogError)error;
}

/*
CommonDialogError WebBrowserDialog::GetResult(CS_OUT int32_t &result, CS_OUT CommonDialogResult &userAction, CS_OUT uint64_t &userData)
{
	SceNpProfileDialogResult params;
	auto error = sceWebBrowserDialogGetResult(&params);
	if (error >= SCE_OK)
	{
		result = params.result;
		userAction = (CommonDialogResult)params.userAction;
		userData = (uint64_t)params.userData;
	}

	return (CommonDialogError)error;
}
*/

CommonDialogStatus WebBrowserDialog::GetStatus()
{
	auto status = sceWebBrowserDialogGetStatus();
	return (CommonDialogStatus)status;
}

CommonDialogStatus WebBrowserDialog::UpdateStatus()
{
	auto status = sceWebBrowserDialogUpdateStatus();
	return (CommonDialogStatus)status;
}