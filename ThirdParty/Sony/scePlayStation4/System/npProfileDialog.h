#pragma once

#include "predecls.h"

#include "commonDialog.h"

#include <np_profile_dialog.h>
#include <sceerror.h>

namespace System {

enum class NpProfileDialogMode
{
	Invalid = SCE_NP_PROFILE_DIALOG_MODE_INVALID,
	Normal = SCE_NP_PROFILE_DIALOG_MODE_NORMAL,
};


class CS_API NpProfileDialog
{
	NpProfileDialog() { }

public:

	static CommonDialogError Initialize();

	static CommonDialogError Terminate();

	static CommonDialogError Open(NpProfileDialogMode mode, UserServiceUserId userId, UserServiceUserId targetOnlineId, uint64_t userData);

	static CommonDialogError Close();

	//static CommonDialogError GetResult(CS_OUT int32_t &result, CS_OUT CommonDialogResult &userAction, CS_OUT uint64_t &userData);

	static CommonDialogStatus GetStatus();

	static CommonDialogStatus UpdateStatus();
};

} // namespace System
