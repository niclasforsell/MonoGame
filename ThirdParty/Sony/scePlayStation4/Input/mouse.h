#pragma once

#include "predecls.h"

namespace Input {

class __declspec(dllexport) Mouse
{
public:
	static void Initialize();
	static void Terminate();
	static void Update();

	static int Enable(SceUserServiceUserId userId);
	static int Disable(SceUserServiceUserId userId);

	static MouseState* GetState(int playerIndex);
};

} // namespace Input
