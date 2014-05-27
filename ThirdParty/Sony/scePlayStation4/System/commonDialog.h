#pragma once

#include "predecls.h"

#include <common_dialog.h>
#include <sceerror.h>

namespace System {

enum class CommonDialogError
{
	Ok = SCE_OK,

	NotInitialized = SCE_COMMON_DIALOG_ERROR_NOT_INITIALIZED,	
	AlreadyInitialized = SCE_COMMON_DIALOG_ERROR_ALREADY_INITIALIZED,	
	NotFinished = SCE_COMMON_DIALOG_ERROR_NOT_FINISHED,	
	InvalidState = SCE_COMMON_DIALOG_ERROR_INVALID_STATE,	
	ResultNone = SCE_COMMON_DIALOG_ERROR_RESULT_NONE,	
	Busy = SCE_COMMON_DIALOG_ERROR_BUSY,	
	OutOfMemory = SCE_COMMON_DIALOG_ERROR_OUT_OF_MEMORY,	
	ParamInvalid = SCE_COMMON_DIALOG_ERROR_PARAM_INVALID,	
	NotRunning = SCE_COMMON_DIALOG_ERROR_NOT_RUNNING,	
	AlreadyClose = SCE_COMMON_DIALOG_ERROR_ALREADY_CLOSE,	
	ArgNull = SCE_COMMON_DIALOG_ERROR_ARG_NULL,	
	UnexpectedFatal = SCE_COMMON_DIALOG_ERROR_UNEXPECTED_FATAL,	
	NotSupported = SCE_COMMON_DIALOG_ERROR_NOT_SUPPORTED,	
};

enum class CommonDialogStatus
{
	None = SCE_COMMON_DIALOG_STATUS_NONE,
	Initialized = SCE_COMMON_DIALOG_STATUS_INITIALIZED,
	Running = SCE_COMMON_DIALOG_STATUS_RUNNING,
	Finished = SCE_COMMON_DIALOG_STATUS_FINISHED,
};

enum class CommonDialogResult
{
	Ok = SCE_COMMON_DIALOG_RESULT_OK,
	Canceled = SCE_COMMON_DIALOG_RESULT_USER_CANCELED,
};


class CS_API CommonDialog
{
	CommonDialog() { }

public:
		
	static CommonDialogError Initialize();

	static bool GetIsUsed();

};

} // namespace System
