#include "npCommerce.h"

#include <libsysmodule.h>


using namespace Network;

void NpCommerce::_tryInitialize()
{
	if (sceSysmoduleIsLoaded(SCE_SYSMODULE_NP_COMMERCE) == SCE_SYSMODULE_LOADED)
		return;

	auto ret = sceSysmoduleLoadModule(SCE_SYSMODULE_NP_COMMERCE);
	if (ret < 0)
	{
		printf("ERROR: Couldn't initialize NpCommerce library: 0x%08X\n", ret);
		return;
	}
}

void NpCommerce::ShowPsStoreIcon(NpCommercePos pos)
{
	_tryInitialize();
	sceNpCommerceShowPsStoreIcon((SceNpCommercePsStoreIconPos)pos);
}

void NpCommerce::HidePsStoreIcon()
{
	_tryInitialize();
	sceNpCommerceHidePsStoreIcon();
}
