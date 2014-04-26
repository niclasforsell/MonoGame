#pragma once

#include "predecls.h"
#include "graphicsEnums.h"

namespace Graphics {


class __declspec(dllexport) RenderTarget
{
	friend class GraphicsSystem;

private:

	sce::Gnm::RenderTarget *_renderTarget;
	sce::Gnm::Texture *_texture;

	sce::Gnm::DepthRenderTarget *_depthTarget;
	bool _hasStencil;

	// No default or copy constructors.
	RenderTarget() { }
	RenderTarget( const RenderTarget & ) { }

	static RenderTarget* Create(TextureFormat format, int32_t width, int32_t height, int32_t slices, DepthFormat depthFormat, bool isCubemap);

public:
	static RenderTarget* Create2D(TextureFormat format, int32_t width, int32_t height, DepthFormat depthFormat);
	static RenderTarget* Create3D(TextureFormat format, int32_t width, int32_t height, int32_t slices, DepthFormat depthFormat);
	static RenderTarget* CreateCube(TextureFormat format, int32_t width, int32_t height, DepthFormat depthFormat);

	virtual ~RenderTarget();

};

} // namespace Graphics
