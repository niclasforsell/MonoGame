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

	// Motion sensor data
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

	// Touch pad data (2 points max)
	// C# doesn't let us build static size arrays,
	// so this is unrolled.
	uint8_t TouchCount;

	float Touch1X;
	float Touch1Y;
	uint8_t Touch1Id;

	float Touch2X;
	float Touch2Y;
	uint8_t Touch2Id;
};

} // namespace Input
