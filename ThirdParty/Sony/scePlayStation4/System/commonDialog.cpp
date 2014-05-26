#include "commonDialog.h"


using namespace System;


CommonDialogError CommonDialog::Initialize()
{
	auto result = sceCommonDialogInitialize();
	return (CommonDialogError)result;
}

bool CommonDialog::GetIsUsed()
{
	return sceCommonDialogIsUsed();
}
