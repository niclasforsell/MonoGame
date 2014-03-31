#pragma once

#include <gnm.h>
#include <gnmx.h>

namespace Graphics {

typedef struct BufferInfo
{
	uint8_t *buffer;
	uint32_t bufferSize;
};

const int MAX_DISCARD_BUFFERS = 1000;


typedef struct DisplayBuffer
{
	sce::Gnmx::GfxContext			context;
	void							*cpRamShadow;
	void							*cueHeap;
	void							*dcbBuffer;
	void							*ccbBuffer;
	sce::Gnm::RenderTarget			renderTarget;
	bool							hasDepthTarget;
	sce::Gnm::DepthRenderTarget		depthTarget;
	volatile uint32_t				*state;

	BufferInfo freeBuffers[MAX_DISCARD_BUFFERS];
	uint32_t freeBufferCount;

	BufferInfo discardBuffers[MAX_DISCARD_BUFFERS];
	uint32_t discardBufferCount;
};

} // namespace Graphics