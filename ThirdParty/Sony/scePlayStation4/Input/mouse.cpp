#include "mouse.h"
#include "mouseState.h"

#include <memory.h>
#include <assert.h>
#include <sceerror.h>
#include <libsysmodule.h>
#include <mouse.h>

using namespace Input;

namespace {
	const int PLAYER_MAX = 4;

	MouseHandle handles[PLAYER_MAX];
	MouseState states[PLAYER_MAX];
}

void Mouse::Initialize()
{
	memset(handles, -1, sizeof(MouseHandle) * PLAYER_MAX);
	memset(states, 0, sizeof(MouseState));

	auto ret = sceSysmoduleLoadModule(SCE_SYSMODULE_MOUSE);
	if (ret < 0)
	{
		printf("ERROR: Couldn't initialize mouse input library: 0x%08X\n", ret);
		return;
	}

	ret = sceMouseInit();
	if (ret != SCE_OK)
	{
		printf("ERROR: Failed to initialize mouse: 0x%08X\n", ret);
	}
}

void Mouse::Terminate()
{
	sceSysmoduleUnloadModule(SCE_SYSMODULE_MOUSE);
}

void Mouse::Update()
{
	// Clear all state by default

	for (auto i = 0; i < PLAYER_MAX; i++)
	{
		auto handle = handles[i];
		if (handle == -1)
			continue;

		auto state = &states[i];
		SceMouseData data;
		memset(&data, 0, sizeof(SceMouseData));
		const int desiredSamples = 1;
		auto numSamples = sceMouseRead(handles[i], &data, desiredSamples);
		if (data.buttons & SCE_MOUSE_BUTTON_INTERCEPTED)
				continue;

		if (numSamples < 0)
		{
			memset(state, 0, sizeof(MouseState));
			printf("ERROR: Couldn't read mouse data! 0x%08X\n", numSamples);
			continue;
		}
		else if (numSamples == 0)
		{
			// No new data to read
			state->XAxis = 0;
			state->YAxis = 0;
			state->Wheel = 0;
			state->Tilt = 0;
		}
		else if(numSamples == desiredSamples)
		{
			state->IsConnected = data.connected;
			state->PacketNumber = data.timestamp;
			state->Buttons = data.buttons;
			state->XAxis = data.xAxis;
			state->YAxis = data.yAxis;
			state->Wheel = data.wheel;
			state->Tilt = data.tilt;
		}
	}
}

int Mouse::Enable(SceUserServiceUserId userId, int playerIndex)
{
	auto handle = sceMouseOpen(userId, SCE_MOUSE_PORT_TYPE_STANDARD, 0, NULL);
	assert(handle >= 0);

	handles[playerIndex] = handle;
	memset(&states[playerIndex], 0, sizeof(MouseState));

	return 0;
}

int Mouse::Disable(int playerIndex)
{
	auto handle = handles[playerIndex];
	auto ret = sceMouseClose(handle);
	assert (ret == SCE_OK);

	handles[playerIndex] = -1;
	memset(&states[playerIndex], 0, sizeof(MouseState));

	return 0;
}

MouseState* Mouse::GetState(int playerIndex)
{
	assert(playerIndex >= 0);
	assert(playerIndex < PLAYER_MAX);

	return &states[playerIndex];
}

