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

	// No default or copy constructors.
	RenderTarget() { }
	RenderTarget( const RenderTarget & ) { }

public:
	RenderTarget(TextureFormat format, int32_t width, int32_t height);
	virtual ~RenderTarget();

};

} // namespace Graphics
