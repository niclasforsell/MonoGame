#pragma once

#include "predecls.h"

#include <np_commerce_dialog.h>
#include <sceerror.h>

namespace System
{
	enum class CommonDialogError;
	enum class CommonDialogStatus;
}


namespace Network {


enum class NpCommerceDialogResult : int32_t
{
	Ok = SCE_COMMON_DIALOG_RESULT_OK,
	UserCanceled = SCE_COMMON_DIALOG_RESULT_USER_CANCELED,
	Purchased = SCE_NP_COMMERCE_DIALOG_RESULT_PURCHASED,
};

CS_FLAGS enum class NpPlusFeature : uint64_t
{
	RealtimeMultiplay = SCE_NP_PLUS_FEATURE_REALTIME_MULTIPLAY,
	AsyncMultiplay = SCE_NP_PLUS_FEATURE_ASYNC_MULTIPLAY,
};


class CS_API NpCommerceDialog
{
private:

	SceNpCommerceDialogParam _params;

public:

	NpCommerceDialog(SceUserServiceUserId userId);
	~NpCommerceDialog();

	System::CommonDialogError OpenCategory(SceNpServiceLabel serviceLabel, const char *nameOrNull);
	System::CommonDialogError OpenProduct(SceNpServiceLabel serviceLabel, const char *name);
	System::CommonDialogError OpenProductCode(const char *codeOrNull);
	System::CommonDialogError OpenCheckout(SceNpServiceLabel serviceLabel);
	System::CommonDialogError OpenDownloadList();
	System::CommonDialogError OpenPlus(NpPlusFeature features);

	System::CommonDialogStatus GetStatus();

	System::CommonDialogStatus UpdateStatus();

	System::CommonDialogError Close();

	System::CommonDialogError GetResult(CS_OUT NpCommerceDialogResult *result, CS_OUT bool *authorized);

};

} // namespace Network
