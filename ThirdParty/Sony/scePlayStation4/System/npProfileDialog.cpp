#include "npProfileDialog.h"
#include <libsysmodule.h>
#include <assert.h>

using namespace System;


CommonDialogError NpProfileDialog::Initialize()
{
	if (sceSysmoduleIsLoaded(SCE_SYSMODULE_NP_PROFILE_DIALOG) != SCE_SYSMODULE_LOADED)
	{
		auto error = sceSysmoduleLoadModule(SCE_SYSMODULE_NP_PROFILE_DIALOG);
		assert(error == SCE_OK);
	}

	auto error = sceNpProfileDialogInitialize();
	return (CommonDialogError)error;
}

CommonDialogError NpProfileDialog::Terminate()
{
	auto error = sceNpProfileDialogTerminate();
	return (CommonDialogError)error;
}

CommonDialogError NpProfileDialog::Open(NpProfileDialogMode mode, UserServiceUserId userId, const char* targetUserName, uint64_t userData)
{
	if (targetUserName == NULL)
		return CommonDialogError::ParamInvalid;

	SceNpOnlineId targetOnlineId;
	strcpy(targetOnlineId.data, targetUserName);

	SceNpProfileDialogParam param;
	sceNpProfileDialogParamInitialize(&param);
	param.mode = (sceNpProfileDialogMode)mode;
	param.userId = (SceUserServiceUserId)userId;
	param.targetOnlineId = targetOnlineId;
	param.userData = (void*)userData;

	auto error = sceNpProfileDialogOpen(&param);
	return (CommonDialogError)error;
}

CommonDialogError NpProfileDialog::Close()
{
	auto error = sceNpProfileDialogClose();
	return (CommonDialogError)error;
}

/*
CommonDialogError NpProfileDialog::GetResult(CS_OUT int32_t &result, CS_OUT CommonDialogResult &userAction, CS_OUT uint64_t &userData)
{
	SceNpProfileDialogResult params;
	auto error = sceNpProfileDialogGetResult(&params);
	if (error >= SCE_OK)
	{
		result = params.result;
		userAction = (CommonDialogResult)params.userAction;
		userData = (uint64_t)params.userData;
	}

	return (CommonDialogError)error;
}
*/

CommonDialogStatus NpProfileDialog::GetStatus()
{
	auto status = sceNpProfileDialogGetStatus();
	return (CommonDialogStatus)status;
}

CommonDialogStatus NpProfileDialog::UpdateStatus()
{
	auto status = sceNpProfileDialogUpdateStatus();
	return (CommonDialogStatus)status;
}