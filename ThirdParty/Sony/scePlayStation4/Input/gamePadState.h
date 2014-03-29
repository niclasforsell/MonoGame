#pragma once

#include "predecls.h"

namespace Input {

struct GamePadState
{
	bool IsConnected;
	int PacketNumber;

	float LeftStickX;
	float LeftStickY;

	float RightStickX;
	float RightStickY;

	float LeftTrigger;
	float RightTrigger;

	uint32_t Buttons;
};

} // namespace Input
