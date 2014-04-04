#pragma once

#include "predecls.h"

namespace Input {

struct CS_VALUE_TYPE GamePadState
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

	float OrientationX;
	float OrientationY;
	float OrientationZ;
	float OrientationW;

	float AccelerationX;
	float AccelerationY;
	float AccelerationZ;

	float AngularVelocityX;
	float AngularVelocityY;
	float AngularVelocityZ;
};

} // namespace Input
