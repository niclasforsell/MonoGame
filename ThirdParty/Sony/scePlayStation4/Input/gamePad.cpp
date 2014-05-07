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

	// From ScePadTouchReference for DUALSHOCK®4 and CUH-ZCT1J/CAP-ZCT1/CAP-ZCT1U
	const float TouchPadNormalizeX = 1.0f / 1919.0f;
	const float TouchPadNormalizeY = 1.0f / 941.0f;

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
	state->Buttons |= (buttons & SCE_PAD_BUTTON_CROSS) != 0 ? 4096 : 0;
	state->Buttons |= (buttons & SCE_PAD_BUTTON_CIRCLE) != 0 ? 8192 : 0;
	state->Buttons |= (buttons & SCE_PAD_BUTTON_SQUARE) != 0 ? 16384 : 0;
	state->Buttons |= (buttons & SCE_PAD_BUTTON_TRIANGLE) != 0 ? 32768 : 0;
	state->Buttons |= (buttons & SCE_PAD_BUTTON_TOUCH_PAD) != 0 ? 2048 : 0;

	// Motion sensor data
	state->OrientationX = data.orientation.x;
	state->OrientationY = data.orientation.y;
	state->OrientationZ = data.orientation.z;
	state->OrientationW = data.orientation.w;

	state->AccelerationX = data.acceleration.x;
	state->AccelerationY = data.acceleration.y;
	state->AccelerationZ = data.acceleration.z;

	state->AngularVelocityX = data.angularVelocity.x;
	state->AngularVelocityY = data.angularVelocity.y;
	state->AngularVelocityZ = data.angularVelocity.z;

	// Touchpad data
	state->TouchCount = data.touchData.touchNum;

	state->Touch1X = data.touchData.touch[0].x * TouchPadNormalizeX;
	state->Touch1Y = data.touchData.touch[0].y * TouchPadNormalizeY;
	state->Touch1Id = data.touchData.touch[0].id;

	state->Touch2X = data.touchData.touch[1].x * TouchPadNormalizeX;
	state->Touch2Y = data.touchData.touch[1].y * TouchPadNormalizeY;
	state->Touch2Id = data.touchData.touch[1].id;
}

void GamePad::Update(float elapsedSeconds)
{
	// Clear all pad state by default
	memset(padStates[0], 0, sizeof(GamePadState) * PLAYER_MAX);

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

bool GamePad::SetLightBar(int playerIndex, uint8_t r, uint8_t g, uint8_t b)
{
	assert(playerIndex >= 0);
	assert(playerIndex < PLAYER_MAX);

	// Clamp to minimum value to avoid SCE_PAD_ERROR_INVALID_COLOR_SETTINGS
	const uint8_t min = 13;
	if (r < min && g < min && b < min)
	{
		r = min;
		g = min;
		b = min;
	}

	struct ScePadColor color = {
		(uint8_t)r,
		(uint8_t)g,
		(uint8_t)b,
	};

	auto ret = scePadSetLightBar(padHandles[playerIndex], &color);
	return ret == SCE_OK;
}

bool GamePad::ResetLightBar(int playerIndex)
{
	assert(playerIndex >= 0);
	assert(playerIndex < PLAYER_MAX);

	auto ret = scePadResetLightBar(padHandles[playerIndex]);
	return ret == SCE_OK;
}

bool GamePad::SetMotionEnabled(int playerIndex, bool value)
{
	assert(playerIndex >= 0);
	assert(playerIndex < PLAYER_MAX);

	auto ret = scePadSetMotionSensorState(padHandles[playerIndex], value);
	return ret == SCE_OK;
}

bool GamePad::SetVelocityDeadbandEnabled(int playerIndex, bool value)
{
	assert(playerIndex >= 0);
	assert(playerIndex < PLAYER_MAX);

	auto ret = scePadSetAngularVelocityDeadbandState(padHandles[playerIndex], value);
	return ret == SCE_OK;
}

bool GamePad::SetTiltCorrectionEnabled(int playerIndex, bool value)
{
	assert(playerIndex >= 0);
	assert(playerIndex < PLAYER_MAX);

	auto ret = scePadSetTiltCorrectionState(padHandles[playerIndex], value);
	return ret == SCE_OK;
}

bool GamePad::ResetOrientation(int playerIndex)
{
	assert(playerIndex >= 0);
	assert(playerIndex < PLAYER_MAX);

	auto ret = scePadResetOrientation(padHandles[playerIndex]);
	return ret == SCE_OK;
}
