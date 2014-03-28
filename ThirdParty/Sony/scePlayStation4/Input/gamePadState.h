#pragma once

#include "predecls.h"

namespace Input {

struct __declspec(dllexport) GamePadState
{
	void Activate();
	void Deactivate();
	void SetState(ScePadData* data);

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
