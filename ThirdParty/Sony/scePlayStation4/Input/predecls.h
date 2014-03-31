#pragma once
#include <stdint.h>

#define CS_VALUE_TYPE

// Include all pre-declarations used from header files.

typedef int32_t ControllerHandle;
typedef int32_t SceUserServiceUserId;

struct ScePadColor;
struct ScePadData;

namespace sce {
}

namespace Input {
	struct CS_VALUE_TYPE GamePadState;
}
