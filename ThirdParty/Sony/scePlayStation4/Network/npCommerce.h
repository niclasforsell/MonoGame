#pragma once

#include "predecls.h"

#include <np.h>
#include <sceerror.h>

namespace Network {

enum class NpCommercePos
{
	Center = SCE_NP_COMMERCE_PS_STORE_ICON_CENTER,
	Left = SCE_NP_COMMERCE_PS_STORE_ICON_LEFT,
	Right = SCE_NP_COMMERCE_PS_STORE_ICON_RIGHT,
};


class CS_API NpCommerce
{
	static void _tryInitialize();

	NpCommerce() { }

public:

	static void ShowPsStoreIcon(NpCommercePos pos);

	static void HidePsStoreIcon();

};

} // namespace Network
