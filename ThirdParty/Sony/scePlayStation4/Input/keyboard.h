#pragma once

#include "predecls.h"

namespace Input {

class CS_API Keyboard
{
	Keyboard() { }

public:
	CS_IGNORE static void Initialize();
	CS_IGNORE static void Terminate();
	CS_IGNORE static void Update();

	CS_IGNORE static int Enable(SceUserServiceUserId userId, int playerIndex);
	CS_IGNORE static int Disable(int playerIndex);

	static KeyboardState* GetState(int playerIndex);
};

} // namespace Input
