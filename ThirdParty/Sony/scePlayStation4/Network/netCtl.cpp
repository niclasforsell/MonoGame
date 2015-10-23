#include "netCtl.h"
#include <assert.h>
#include <memory.h>

using namespace Network;

NetCtrlResult NetCtl::GetState(CS_OUT NetCtrlState *state)
{
	assert(state != nullptr);
	auto err = sceNetCtlGetState((int*)state);
	return (NetCtrlResult)err;
}

