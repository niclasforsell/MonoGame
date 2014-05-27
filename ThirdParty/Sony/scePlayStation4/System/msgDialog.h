#pragma once

#include "predecls.h"

#include "commonDialog.h"

#include <message_dialog.h>
#include <sceerror.h>

namespace System {

enum class MsgDialogButtonType
{
	Ok = SCE_MSG_DIALOG_BUTTON_TYPE_OK,
	YesNo = SCE_MSG_DIALOG_BUTTON_TYPE_YESNO,
	None = SCE_MSG_DIALOG_BUTTON_TYPE_NONE,
	OkCancel = SCE_MSG_DIALOG_BUTTON_TYPE_OK_CANCEL,
	Cancel = SCE_MSG_DIALOG_BUTTON_TYPE_CANCEL,
};

enum class MsgDialogButtonId
{
	Invalid = SCE_MSG_DIALOG_BUTTON_ID_INVALID,
	Ok = SCE_MSG_DIALOG_BUTTON_ID_OK,
	Yes = SCE_MSG_DIALOG_BUTTON_ID_YES,
	No = SCE_MSG_DIALOG_BUTTON_ID_NO,
};

enum class MsgDialogSysMsgType
{
	EmptyStore = SCE_MSG_DIALOG_SYSMSG_TYPE_TRC_EMPTY_STORE,
	PsnChatRestriction = SCE_MSG_DIALOG_SYSMSG_TYPE_TRC_PSN_CHAT_RESTRICTION,
	PsnUgcRestriction = SCE_MSG_DIALOG_SYSMSG_TYPE_TRC_PSN_UGC_RESTRICTION,
	WaringSwitchToSimulView = SCE_MSG_DIALOG_SYSMSG_TYPE_TRC_WARNING_SWITCH_TO_SIMULVIEW,
};


class CS_API MsgDialog
{
	MsgDialog() { }

public:

	static CommonDialogError Initialize();

	static CommonDialogError Terminate();

	static CommonDialogError OpenUserMsg(const char *message, MsgDialogButtonType buttonType, UserServiceUserId userId);

	static CommonDialogError OpenProgressBar(const char *message, UserServiceUserId userId);

	static CommonDialogError OpenSystemMsg(MsgDialogSysMsgType sysMsgType, UserServiceUserId userId);

	static CommonDialogError Close();

	//static CommonDialogError GetResult(int dummy, CS_OUT CommonDialogResult &result, CS_OUT MsgDialogButtonId &buttonId);

	static CommonDialogError ProgressBarInc(uint32_t delta);

	static CommonDialogError ProgressBarSetMsg(const char* message);

	static CommonDialogError ProgressBarSetValue(uint32_t rate);

	static CommonDialogStatus GetStatus();

	static CommonDialogStatus UpdateStatus();
};

} // namespace System
