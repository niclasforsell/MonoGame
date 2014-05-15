#pragma once

#include "predecls.h"

namespace Input {

class __declspec(dllexport) GamePad
{
public:
	CS_IGNORE static void Initialize();
	CS_IGNORE static void Terminate();
	CS_IGNORE static void Update();

	CS_IGNORE static int Enable(SceUserServiceUserId userId, int playerIndex);
	CS_IGNORE static int Disable(int playerIndex);

	static GamePadState* GetState(int playerIndex);
	static bool SetVibration(int playerIndex, float smallMotor, float largeMotor);

	// Light bar
	static bool SetLightBar(int playerIndex, uint8_t r, uint8_t g, uint8_t b);
	static bool ResetLightBar(int playerIndex);

	// Motion control
	static bool SetMotionEnabled(int playerIndex, bool value);
	static bool SetVelocityDeadbandEnabled(int playerIndex, bool value);
	static bool SetTiltCorrectionEnabled(int playerIndex, bool value);
	static bool ResetOrientation(int playerIndex);
};

} // namespace Input
