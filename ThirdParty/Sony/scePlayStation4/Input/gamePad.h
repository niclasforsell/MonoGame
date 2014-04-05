#pragma once

#include "predecls.h"

namespace Input {

class __declspec(dllexport) GamePad
{
public:
	static void Initialize();
	static void Terminate();
	static void Update(float elapsedSeconds);

	static int Enable(SceUserServiceUserId userId);
	static int Disable(SceUserServiceUserId userId);

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
