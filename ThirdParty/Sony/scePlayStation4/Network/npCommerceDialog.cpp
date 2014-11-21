#include "npCommerceDialog.h"
#include "..\System\commonDialog.h"
#include <libsysmodule.h>
#include <assert.h>

using namespace Network;
using System::CommonDialogError;
using System::CommonDialogStatus;


NpCommerceDialog::NpCommerceDialog(SceUserServiceUserId userId)
{
	sceNpCommerceDialogParamInitialize(&_params);
	_params.userId = userId;

	sceSysmoduleLoadModule(SCE_SYSMODULE_NP_COMMERCE);
	auto error = sceCommonDialogInitialize();
	assert(error == SCE_OK || error == SCE_COMMON_DIALOG_ERROR_ALREADY_SYSTEM_INITIALIZED);
	error = sceNpCommerceDialogInitialize();
	assert(error == SCE_OK);
}

NpCommerceDialog::~NpCommerceDialog()
{
	auto error = sceNpCommerceDialogTerminate();
	assert(error == SCE_OK);
}

CommonDialogError NpCommerceDialog::OpenCategory(SceNpServiceLabel serviceLabel, const char *nameOrNull)
{
	_params.mode = SCE_NP_COMMERCE_DIALOG_MODE_CATEGORY;
	_params.serviceLabel = serviceLabel;
	const char *category_label[] = { nameOrNull };
	if (nameOrNull && nameOrNull[0])
	{
		_params.targets = category_label;
		_params.numTargets = 1;
	}
	auto error = sceNpCommerceDialogOpen(&_params);
	return (CommonDialogError)error; 
}

CommonDialogError NpCommerceDialog::OpenProduct(SceNpServiceLabel serviceLabel, const char *name)
{
	_params.mode = SCE_NP_COMMERCE_DIALOG_MODE_PRODUCT;
	_params.serviceLabel = serviceLabel;
	const char *product_label[] = { name };
	_params.targets = product_label;
	_params.numTargets = 1;
	auto error = sceNpCommerceDialogOpen(&_params);
	return (CommonDialogError)error;
}

CommonDialogError NpCommerceDialog::OpenProductCode(const char *codeOrNull)
{
	_params.mode = SCE_NP_COMMERCE_DIALOG_MODE_PRODUCT_CODE;
	const char *product_code[] = { codeOrNull };
	if (codeOrNull && codeOrNull[0])
	{
		_params.targets = product_code;
		_params.numTargets = 1;
	}
	auto error = sceNpCommerceDialogOpen(&_params);
	return (CommonDialogError)error;
}

CommonDialogError NpCommerceDialog::OpenCheckout(SceNpServiceLabel serviceLabel)
{
	_params.mode = SCE_NP_COMMERCE_DIALOG_MODE_CHECKOUT;
	_params.serviceLabel = serviceLabel;
	auto error = sceNpCommerceDialogOpen(&_params);
	return (CommonDialogError)error;
}

CommonDialogError NpCommerceDialog::OpenDownloadList()
{
	_params.mode = SCE_NP_COMMERCE_DIALOG_MODE_DOWNLOADLIST;
	auto error = sceNpCommerceDialogOpen(&_params);
	return (CommonDialogError)error;
}

CommonDialogError NpCommerceDialog::OpenPlus(NpPlusFeature features)
{
	_params.mode = SCE_NP_COMMERCE_DIALOG_MODE_PLUS;
	_params.features = (uint64_t)features;
	auto error = sceNpCommerceDialogOpen(&_params);
	return (CommonDialogError)error;
}

CommonDialogError NpCommerceDialog::Close()
{
	auto error = sceNpCommerceDialogClose();
	return (CommonDialogError)error;
}

CommonDialogStatus NpCommerceDialog::GetStatus()
{
	auto status = sceNpCommerceDialogGetStatus();
	return (CommonDialogStatus)status;
}

CommonDialogStatus NpCommerceDialog::UpdateStatus()
{
	auto status = sceNpCommerceDialogUpdateStatus();
	return (CommonDialogStatus)status;
}

CommonDialogError NpCommerceDialog::GetResult(CS_OUT NpCommerceDialogResult *result, CS_OUT bool *authorized)
{
	SceNpCommerceDialogResult result_;
	memset(&result_, 0, sizeof(result_));
	auto error = sceNpCommerceDialogGetResult(&result_);

	*result = (NpCommerceDialogResult)result_.result;
	*authorized = result_.authorized;

	return (CommonDialogError)error;
}
