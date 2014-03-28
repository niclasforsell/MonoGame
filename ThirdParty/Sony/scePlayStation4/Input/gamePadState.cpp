#pragma once

#include "gamePadState.h"
#include <pad.h>
#include <math.h>

using namespace Input;

namespace {
	const float ByteToFloatConvert = 1.0f / 255.0f;
}

void GamePadState::Activate()
{
	IsConnected = true;
}

void GamePadState::Deactivate()
{
	IsConnected = false;
	LeftStickX = 0.0f;
	LeftStickY = 0.0f;
	RightStickX = 0.0f;
	RightStickY = 0.0f;
	LeftTrigger = 0.0f;
	RightTrigger = 0.0f;
	Buttons = 0;
}

float scaleStickValue(uint8_t value)
{
	auto scaledValue = (float)((int32_t)value * 2 - 255) * ByteToFloatConvert;
	if ((fabsf(scaledValue) < 0.06666667f))
		scaledValue = 0.0f;

	return scaledValue;
}

void GamePadState::SetState(ScePadData* data)
{
	IsConnected = data->connected;
	PacketNumber = (uint32_t)data->timestamp;

	LeftStickX = scaleStickValue(data->leftStick.x);
	LeftStickY = -scaleStickValue(data->leftStick.y);

	RightStickX = scaleStickValue(data->rightStick.x);
	RightStickY = -scaleStickValue(data->rightStick.y);

	LeftTrigger = data->analogButtons.l2 / 255.0f;
	RightTrigger = data->analogButtons.r2 / 255.0f;

	auto buttons = data->buttons;
	Buttons |= (buttons & SCE_PAD_BUTTON_UP) != 0 ? 1 : 0;
	Buttons |= (buttons & SCE_PAD_BUTTON_DOWN) != 0 ? 2 : 0;
	Buttons |= (buttons & SCE_PAD_BUTTON_LEFT) != 0 ? 4 : 0;
	Buttons |= (buttons & SCE_PAD_BUTTON_RIGHT) != 0 ? 8 : 0;
	Buttons |= (buttons & SCE_PAD_BUTTON_OPTIONS) != 0 ? 16 : 0;
	//Buttons |= (data.buttons & BACK????) != 0 ? 32 : 0;
	Buttons |= (buttons & SCE_PAD_BUTTON_L3) != 0 ? 64 : 0;
	Buttons |= (buttons & SCE_PAD_BUTTON_R3) != 0 ? 128 : 0;
	Buttons |= (buttons & SCE_PAD_BUTTON_L2) != 0 ?  8388608 : 0;
	Buttons |= (buttons & SCE_PAD_BUTTON_R2) != 0 ? 4194304 : 0;
	Buttons |= (buttons & SCE_PAD_BUTTON_L1) != 0 ? 256 : 0;
	Buttons |= (buttons & SCE_PAD_BUTTON_R1) != 0 ? 512 : 0;
	Buttons |= (buttons & SCE_PAD_BUTTON_L2) != 0 ?  8388608 : 0;
	Buttons |= (buttons & SCE_PAD_BUTTON_R2) != 0 ? 4194304 : 0;
	Buttons |= (buttons & SCE_PAD_BUTTON_CROSS) != 0 ? 4096 : 0;
	Buttons |= (buttons & SCE_PAD_BUTTON_CIRCLE) != 0 ? 8192 : 0;
	Buttons |= (buttons & SCE_PAD_BUTTON_SQUARE) != 0 ? 16384 : 0;
	Buttons |= (buttons & SCE_PAD_BUTTON_TRIANGLE) != 0 ? 32768 : 0;
}
