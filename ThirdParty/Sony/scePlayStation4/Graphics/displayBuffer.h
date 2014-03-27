#pragma once

#include <gnm.h>
#include <gnmx.h>

namespace Graphics {


typedef struct DisplayBuffer
{
	sce::Gnmx::GfxContext			context;
	void							*cpRamShadow;
	void							*cueHeap;
	void							*dcbBuffer;
	void							*ccbBuffer;
	sce::Gnm::RenderTarget			renderTarget;
	sce::Gnm::DepthRenderTarget		depthTarget;
	volatile uint32_t				*state;

	/*
	// Gpu mapped buffer space for vertex and index data.
	sce::Gnm::Buffer userBuffer[3];
	uint32_t userOffset;
	void *userData;

	// 1MB should be a safe scratch buffer size for all a
	// scenese vertex/index data.
	static const uint32_t MaxBufferSize = 1024 * 1024;
	*/
};

} // namespace Graphics