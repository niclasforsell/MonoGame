#include "gamePad.h"
#include "gamePadState.h"

#include <memory.h>
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

	ControllerHandle handles[PLAYER_MAX];
	GamePadState states[PLAYER_MAX];

	float scaleStickValue(uint8_t value)
	{
		auto scaledValue = (float)((int32_t)value * 2 - 255) * ByteToFloatConvert;
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
}

void GamePad::Initialize()
{
	memset(handles, -1, sizeof(ControllerHandle) * PLAYER_MAX);
	memset(states, 0, sizeof(GamePadState) * PLAYER_MAX);

	auto ret = scePadInit();
	assert(ret == SCE_OK);
}

void GamePad::Terminate()
{
	for (auto i = 0; i < PLAYER_MAX; i++)
		Disable(i);
}

void GamePad::Update()
{
	// Clear all pad state by default
	memset(states, 0, sizeof(GamePadState) * PLAYER_MAX);

	for (auto i = 0; i < PLAYER_MAX; i++)
	{
		auto handle = handles[i];
		if (handle == -1)
			continue;

		ScePadData data;
		auto ret = scePadReadState(handles[i], &data);
		if (ret == SCE_OK)
			copyState(data, &states[i]);
	}
}

int GamePad::Enable(SceUserServiceUserId userId, int playerIndex)
{
	// Is this user already enabled?
	if (handles[playerIndex] != -1)
		return 0;

	auto handle = scePadOpen(userId, SCE_PAD_PORT_TYPE_STANDARD, 0, NULL);
	assert(handle >= 0);
	if (handle < 0)
	{
		printf("ERROR: Couldn't open gamepad for user %d playerIndex %d\n", userId, playerIndex);
		return -1;
	}

	handles[playerIndex] = handle;

	return 0;
}

int GamePad::Disable(int playerIndex)
{
	auto handle = handles[playerIndex];
	if (handle == -1)
		return -1;

	auto ret = scePadClose(handle);
	assert (ret == SCE_OK);

	handles[playerIndex] = -1;
	memset(&states[playerIndex], 0, sizeof(GamePadState));

	return ret;
}

GamePadState* GamePad::GetState(int playerIndex)
{
	assert(playerIndex >= 0);
	assert(playerIndex < PLAYER_MAX);

	return &states[playerIndex];
}

bool GamePad::SetVibration(int playerIndex, float smallMotor, float largeMotor)
{
	assert(playerIndex >= 0);
	assert(playerIndex < PLAYER_MAX);

	if (!states[playerIndex].IsConnected)
		return false;

	auto vibParams = ScePadVibrationParam();
	vibParams.largeMotor = largeMotor * 255;
	vibParams.smallMotor = smallMotor * 255;

	auto ret = scePadSetVibration(handles[playerIndex], &vibParams);
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

	auto ret = scePadSetLightBar(handles[playerIndex], &color);
	return ret == SCE_OK;
}

bool GamePad::ResetLightBar(int playerIndex)
{
	assert(playerIndex >= 0);
	assert(playerIndex < PLAYER_MAX);

	auto ret = scePadResetLightBar(handles[playerIndex]);
	return ret == SCE_OK;
}

bool GamePad::SetMotionEnabled(int playerIndex, bool value)
{
	assert(playerIndex >= 0);
	assert(playerIndex < PLAYER_MAX);

	auto ret = scePadSetMotionSensorState(handles[playerIndex], value);
	return ret == SCE_OK;
}

bool GamePad::SetVelocityDeadbandEnabled(int playerIndex, bool value)
{
	assert(playerIndex >= 0);
	assert(playerIndex < PLAYER_MAX);

	auto ret = scePadSetAngularVelocityDeadbandState(handles[playerIndex], value);
	return ret == SCE_OK;
}

bool GamePad::SetTiltCorrectionEnabled(int playerIndex, bool value)
{
	assert(playerIndex >= 0);
	assert(playerIndex < PLAYER_MAX);

	auto ret = scePadSetTiltCorrectionState(handles[playerIndex], value);
	return ret == SCE_OK;
}

bool GamePad::ResetOrientation(int playerIndex)
{
	assert(playerIndex >= 0);
	assert(playerIndex < PLAYER_MAX);

	auto ret = scePadResetOrientation(handles[playerIndex]);
	return ret == SCE_OK;
}
