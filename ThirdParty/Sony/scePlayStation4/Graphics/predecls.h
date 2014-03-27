#pragma once
#include <stdint.h>

// Include all pre-declarations used from header files.

struct _SceKernelEqueue;

/*
typedef int32_t ControllerHandle;

struct SceSaveDataMountPoint;
struct SceSaveDataMount;
struct SceSaveDataDirName;
struct ScePadColor;
*/

namespace sce {

	namespace Gnm {
		class Buffer;
		class Texture;
		class RenderTarget;
		class DataFormat;
	}

	namespace Gnmx {

		class VsShader;
		class PsShader;
		class GfxContext;

	}
}

/*
class PS4GamePadState;
class AudioOut;
class ScePadData;
class Effect;
class GamePadManager;
class PS4GamePadState;
*/

namespace Graphics {

class Texture;
class RenderTarget;

struct DisplayBuffer;

}

#include "textureFormat.h"