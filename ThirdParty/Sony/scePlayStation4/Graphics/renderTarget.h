#pragma once

#include "predecls.h"

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

protected:
	RenderTarget(sce::Gnm::RenderTarget *renderTarget);

public:
	virtual ~RenderTarget();

	uint32_t getWidth();

	uint32_t getHeight();
};

} // namespace Graphics
