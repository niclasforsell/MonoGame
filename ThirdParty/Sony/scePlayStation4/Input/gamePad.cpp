#include "gamePad.h"

#include <assert.h>
#include <sceerror.h>
#include <pad.h>

using namespace Input;

namespace {
	const int PLAYER_MAX = 4;
	int activeUsers = 0;

	ControllerHandle padHandles[PLAYER_MAX];
	GamePadState padStates[PLAYER_MAX];
	SceUserServiceUserId padUsers[PLAYER_MAX];
}

void GamePad::Initialize()
{
	activeUsers = 0;
	for (auto i = 0; i < PLAYER_MAX; i++)
	{
		padHandles[i] = -1;
		padStates[i].Deactivate();
		padUsers[i] = -1;
	}

	auto ret = scePadInit();
	assert(ret == SCE_OK);
}

void GamePad::Terminate()
{
	for (auto i = 0; i < PLAYER_MAX; i++)
		padStates[i].Deactivate();

	activeUsers = 0;
}

void GamePad::Update(float elapsedSeconds)
{
	for (auto i = 0; i < PLAYER_MAX; i++)
	{
		auto state = padStates[i];
		if (!state.IsConnected)
			continue;

		ScePadData padData;
		auto ret = scePadReadState(padHandles[i], &padData);
		assert(ret == SCE_OK);

		state.SetState(&padData);
	}
}

int GamePad::Enable(SceUserServiceUserId userId)
{
	// Is this user already enabled?
	for (auto i = 0; i < PLAYER_MAX; i++)
	{
		if (padUsers[i] == userId)
			return i;
	}
	
	assert(activeUsers < PLAYER_MAX);

	auto handle = scePadOpen(userId, SCE_PAD_PORT_TYPE_STANDARD, 0, NULL);
	assert(handle >= 0);

	auto playerIndex = activeUsers;
	padHandles[playerIndex] = handle;
	padStates[playerIndex].Activate();
	padUsers[playerIndex] = userId;
	activeUsers++;

	return playerIndex;
}

int GamePad::Disable(SceUserServiceUserId userId)
{
	for (auto i = 0; i < PLAYER_MAX; i++)
	{
		if (padUsers[i] != userId)
			continue;

		auto handle = padHandles[i];
		auto ret = scePadClose(handle);
		assert (ret == SCE_OK);

		padHandles[i] = -1;
		padStates[i].Deactivate();
		padUsers[i] = -1;
		activeUsers--;

		return i;
	}

	return -1;
}

GamePadState* GamePad::GetState(int playerIndex)
{
	assert(playerIndex >= 0);
	assert(playerIndex < PLAYER_MAX);

	return &padStates[playerIndex];
}

void GamePad::SetColor(int playerIndex, uint8_t r, uint8_t g, uint8_t b)
{
	assert(playerIndex >= 0);
	assert(playerIndex < PLAYER_MAX);

	if (!padStates[playerIndex].IsConnected)
		return;

	struct ScePadColor color = {
		(uint8_t)r,
		(uint8_t)g,
		(uint8_t)b,
	};

	scePadSetLightBar(padHandles[playerIndex], &color);
}
