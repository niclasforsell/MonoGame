#pragma once

#include "predecls.h"

namespace Input {

class __declspec(dllexport) Keyboard
{
public:
	static void Initialize();
	static void Terminate();
	static void Restart();
	static void Update();

	static int Enable(SceUserServiceUserId userId);
	static int Disable(SceUserServiceUserId userId);

	static KeyboardState* GetState(int playerIndex);
};

} // namespace Input
