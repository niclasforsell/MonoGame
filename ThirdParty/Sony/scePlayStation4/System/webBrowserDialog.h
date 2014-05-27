#pragma once

#include "predecls.h"

#include "commonDialog.h"

#include <web_browser_dialog.h>
#include <sceerror.h>

namespace System {


	
class CS_API WebBrowserDialog
{
	WebBrowserDialog() { }

public:

	static CommonDialogError Initialize();

	static CommonDialogError Terminate();

	static CommonDialogError Open(const char *url, UserServiceUserId userId);

	static CommonDialogError Close();

	//static CommonDialogError GetResult(CS_OUT int32_t &result, CS_OUT CommonDialogResult &userAction, CS_OUT uint64_t &userData);

	static CommonDialogStatus GetStatus();

	static CommonDialogStatus UpdateStatus();
};

} // namespace System
