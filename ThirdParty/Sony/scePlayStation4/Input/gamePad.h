#pragma once

#include "predecls.h"
#include "gamePadState.h"

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

	static void SetColor(int playerIndex, uint8_t r, uint8_t g, uint8_t b);
};

} // namespace Input
