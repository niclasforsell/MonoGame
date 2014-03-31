#pragma once
#include <stdint.h>

#define CS_IGNORE
#define CS_OUT

// Include all pre-declarations used from header files.

struct _SceKernelEqueue;

namespace sce {

	namespace Gnm {
		class Buffer;
		class Texture;
		class RenderTarget;
		class DepthRenderTarget;
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
class GraphicsSystem;

struct DisplayBuffer;

}
