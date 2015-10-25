#pragma once

#include "predecls.h"
#include <scebase.h>
#include <libnetctl.h>

namespace Network {

enum class NetCtrlResult
{
	Ok = SCE_OK,

	ErrorCallbackMax = SCE_NET_CTL_ERROR_CALLBACK_MAX,
	ErrorIdNotFound = SCE_NET_CTL_ERROR_ID_NOT_FOUND,
	ErrorInvalidId = SCE_NET_CTL_ERROR_INVALID_ID,
	ErrorInvalidCode = SCE_NET_CTL_ERROR_INVALID_CODE,
	ErrorInvalidAddr = SCE_NET_CTL_ERROR_INVALID_ADDR,
	ErrorNotConnected = SCE_NET_CTL_ERROR_NOT_CONNECTED,
	ErrorNotAvail = SCE_NET_CTL_ERROR_NOT_AVAIL,
	ErrorNetworkDisabled = SCE_NET_CTL_ERROR_NETWORK_DISABLED,
	ErrorDisconnectReq = SCE_NET_CTL_ERROR_DISCONNECT_REQ,
	ErrorInvalidType = SCE_NET_CTL_ERROR_INVALID_TYPE,
	ErrorInvalidSize = SCE_NET_CTL_ERROR_INVALID_SIZE,
	ErrorEthernetPlugout = SCE_NET_CTL_ERROR_ETHERNET_PLUGOUT,
	ErrorWifiDeauthed = SCE_NET_CTL_ERROR_WIFI_DEAUTHED,
	ErrorWifiBeaconLost = SCE_NET_CTL_ERROR_WIFI_BEACON_LOST,
};

enum class NetCtrlState : int
{
	Disconnected = SCE_NET_CTL_STATE_DISCONNECTED,
	Connecting = SCE_NET_CTL_STATE_CONNECTING,
	IpObtaining = SCE_NET_CTL_STATE_IPOBTAINING,
	IpObtained = SCE_NET_CTL_STATE_IPOBTAINED,
};


class CS_API NetCtl
{
	NetCtl() {}
	~NetCtl() {}

public:

	static NetCtrlResult GetState(CS_OUT NetCtrlState *state);
};

} // namespace Network
