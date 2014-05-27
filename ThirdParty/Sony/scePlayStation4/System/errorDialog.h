#pragma once

#include "predecls.h"

#include <error_dialog.h>
#include <sceerror.h>

namespace System {

enum class ErrorDialogError
{
	Ok = SCE_OK,

	NotInitialized = SCE_ERROR_DIALOG_ERROR_NOT_INITIALIZED,	
	AlreadyInitialized = SCE_ERROR_DIALOG_ERROR_ALREADY_INITIALIZED,	
	InvalidState = SCE_ERROR_DIALOG_ERROR_INVALID_STATE,	
	ServiceBusy = SCE_ERROR_DIALOG_ERROR_SERVICE_BUSY,	
	ParamInvalid = SCE_ERROR_DIALOG_ERROR_PARAM_INVALID,	
	UnexpectedFatal = SCE_ERROR_DIALOG_ERROR_UNEXPECTED_FATAL,	
	InvalidUserId = SCE_ERROR_DIALOG_ERROR_INVALID_USER_ID,	
};

enum class ErrorDialogStatus
{
	None = SCE_ERROR_DIALOG_STATUS_NONE,
	Initialized = SCE_ERROR_DIALOG_STATUS_INITIALIZED,
	Running = SCE_ERROR_DIALOG_STATUS_RUNNING,
	Finished = SCE_ERROR_DIALOG_STATUS_FINISHED,
};


class CS_API ErrorDialog
{
	ErrorDialog() { }

public:

	static ErrorDialogError Initialize();

	static ErrorDialogError Terminate();

	static ErrorDialogError Open(int32_t errorCode, UserServiceUserId userId);

	static ErrorDialogError Close();

	static ErrorDialogStatus GetStatus();

	static ErrorDialogStatus UpdateStatus();
};

} // namespace System
