#pragma once
#include <stdint.h>

#define CS_VALUE_TYPE

// Include all pre-declarations used from header files.

typedef int32_t ControllerHandle;
typedef int32_t MouseHandle;
typedef int32_t SceUserServiceUserId;

namespace Input {
	struct CS_VALUE_TYPE GamePadState;
	class GamePad;

	struct CS_VALUE_TYPE KeyboardState;
	class Keyboard;

	struct CS_VALUE_TYPE MouseState;
	class Mouse;
}
