#include "gamePad.h"
#include "gamePadState.h"
#include "../allocator.h"

#include <math.h>
#include <assert.h>
#include <sceerror.h>
#include <pad.h>

using namespace Input;

namespace {
	const int PLAYER_MAX = 4;
	const float ByteToFloatConvert = 1.0f / 255.0f;

	ControllerHandle padHandles[PLAYER_MAX];
	GamePadState* padStates[PLAYER_MAX];
	SceUserServiceUserId padUsers[PLAYER_MAX];
}

void GamePad::Initialize()
{
	for (auto i = 0; i < PLAYER_MAX; i++)
	{
		padHandles[i] = -1;
		padStates[i] = (GamePadState*)Allocator::Get()->allocate(sizeof(GamePadState));
		memset(padStates[i], 0, sizeof(GamePadState));
		padUsers[i] = -1;
	}

	auto ret = scePadInit();
	assert(ret == SCE_OK);
}

void GamePad::Terminate()
{
	for (auto i = 0; i < PLAYER_MAX; i++)
		Allocator::Get()->release(padStates[i]);
}

int findOpenSlot()
{
	for (auto i = 0; i < PLAYER_MAX; i++)
	{
		if (padHandles[i] == -1)
			return i;
	}

	return -1;
}

float scaleStickValue(uint8_t value)
{
	auto scaledValue = (float)((int32_t)value * 2 - 255) * ByteToFloatConvert;
	if ((fabsf(scaledValue) < 0.06666667f))
		scaledValue = 0.0f;

	return scaledValue;
}

void clearState(GamePadState* state)
{
	state->IsConnected = false;
	state->LeftStickX = 0.0f;
	state->LeftStickY = 0.0f;
	state->RightStickX = 0.0f;
	state->RightStickY = 0.0f;
	state->LeftTrigger = 0.0f;
	state->RightTrigger = 0.0f;
}

void copyState(const ScePadData& data, GamePadState* state)
{
	state->IsConnected = data.connected;
	state->PacketNumber = (uint32_t)data.timestamp;

	state->LeftStickX = scaleStickValue(data.leftStick.x);
	state->LeftStickY = -scaleStickValue(data.leftStick.y);

	state->RightStickX = scaleStickValue(data.rightStick.x);
	state->RightStickY = -scaleStickValue(data.rightStick.y);

	state->LeftTrigger = data.analogButtons.l2 / 255.0f;
	state->RightTrigger = data.analogButtons.r2 / 255.0f;

	auto buttons = data.buttons;
	state->Buttons = 0;
	state->Buttons |= (buttons & SCE_PAD_BUTTON_UP) != 0 ? 1 : 0;
	state->Buttons |= (buttons & SCE_PAD_BUTTON_DOWN) != 0 ? 2 : 0;
	state->Buttons |= (buttons & SCE_PAD_BUTTON_LEFT) != 0 ? 4 : 0;
	state->Buttons |= (buttons & SCE_PAD_BUTTON_RIGHT) != 0 ? 8 : 0;
	state->Buttons |= (buttons & SCE_PAD_BUTTON_OPTIONS) != 0 ? 16 : 0;
	//state->Buttons |= (data.buttons & BACK????) != 0 ? 32 : 0;
	state->Buttons |= (buttons & SCE_PAD_BUTTON_L3) != 0 ? 64 : 0;
	state->Buttons |= (buttons & SCE_PAD_BUTTON_R3) != 0 ? 128 : 0;
	state->Buttons |= (buttons & SCE_PAD_BUTTON_L2) != 0 ?  8388608 : 0;
	state->Buttons |= (buttons & SCE_PAD_BUTTON_R2) != 0 ? 4194304 : 0;
	state->Buttons |= (buttons & SCE_PAD_BUTTON_L1) != 0 ? 256 : 0;
	state->Buttons |= (buttons & SCE_PAD_BUTTON_R1) != 0 ? 512 : 0;
	state->Buttons |= (buttons & SCE_PAD_BUTTON_L2) != 0 ?  8388608 : 0;
	state->Buttons |= (buttons & SCE_PAD_BUTTON_R2) != 0 ? 4194304 : 0;
	state->Buttons |= (buttons & SCE_PAD_BUTTON_CROSS) != 0 ? 4096 : 0;
	state->Buttons |= (buttons & SCE_PAD_BUTTON_CIRCLE) != 0 ? 8192 : 0;
	state->Buttons |= (buttons & SCE_PAD_BUTTON_SQUARE) != 0 ? 16384 : 0;
	state->Buttons |= (buttons & SCE_PAD_BUTTON_TRIANGLE) != 0 ? 32768 : 0;
}

void GamePad::Update(float elapsedSeconds)
{
	for (auto i = 0; i < PLAYER_MAX; i++)
	{
		auto handle = padHandles[i];
		if (handle == -1)
			continue;

		auto state = padStates[i];
		ScePadData data;
		auto ret = scePadReadState(padHandles[i], &data);
		if (ret == SCE_OK)
			copyState(data, state);
		else
			clearState(state);
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

	auto playerIndex = findOpenSlot();
	if (playerIndex >= 0)
	{
		auto handle = scePadOpen(userId, SCE_PAD_PORT_TYPE_STANDARD, 0, NULL);
		assert(handle >= 0);

		padHandles[playerIndex] = handle;
		clearState(padStates[playerIndex]);
		padUsers[playerIndex] = userId;
	}

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
		clearState(padStates[i]);
		padUsers[i] = -1;

		return i;
	}

	return -1;
}

GamePadState* GamePad::GetState(int playerIndex)
{
	assert(playerIndex >= 0);
	assert(playerIndex < PLAYER_MAX);

	return padStates[playerIndex];
}

bool GamePad::SetColor(int playerIndex, uint8_t r, uint8_t g, uint8_t b)
{
	assert(playerIndex >= 0);
	assert(playerIndex < PLAYER_MAX);

	if (!padStates[playerIndex]->IsConnected)
		return false;

	struct ScePadColor color = {
		(uint8_t)r,
		(uint8_t)g,
		(uint8_t)b,
	};

	auto ret = scePadSetLightBar(padHandles[playerIndex], &color);
	return ret == SCE_OK;
}

bool GamePad::SetVibration(int playerIndex, float smallMotor, float largeMotor)
{
	assert(playerIndex >= 0);
	assert(playerIndex < PLAYER_MAX);

	if (!padStates[playerIndex]->IsConnected)
		return false;

	auto vibParams = ScePadVibrationParam();
	vibParams.largeMotor = largeMotor * 255;
	vibParams.smallMotor = smallMotor * 255;

	auto ret = scePadSetVibration(padHandles[playerIndex], &vibParams);
	return ret == SCE_OK;
}

