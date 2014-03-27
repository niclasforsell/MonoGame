#pragma once
#include <stdint.h>

// Include all pre-declarations used from header files.

struct _SceKernelEqueue;

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

namespace Graphics {

class Texture;
class RenderTarget;
class VertexShader;
class PixelShader;
class VertexBuffer;
class IndexBuffer;

struct DisplayBuffer;

}

#include "textureFormat.h"