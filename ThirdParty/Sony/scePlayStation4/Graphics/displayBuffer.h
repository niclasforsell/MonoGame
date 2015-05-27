#pragma once

#include <gnm.h>
#include <gnmx.h>

namespace Graphics {

struct BufferInfo
{
	uint8_t *buffer;
	uint32_t bufferSize;
};

const int MAX_DISCARD_BUFFERS = 1000;


class DisplayBuffer
{
public:
	sce::Gnmx::GfxContext			context;
	void							*cpRamShadow;
	void							*cueHeap;
	void							*dcbBuffer;
	void							*ccbBuffer;
	sce::Gnm::RenderTarget			renderTarget;
	bool							hasDepthTarget;
	sce::Gnm::DepthRenderTarget		depthTarget;
	volatile uint32_t				*state;

	VertexBuffer *currentVB;
	bool currentVBDirty;
	uint32_t currentVBOffset;

	IndexBuffer *currentIB;
	bool currentIBDirty;

	BufferInfo freeBuffers[MAX_DISCARD_BUFFERS];
	uint32_t freeBufferCount;

	BufferInfo discardBuffers[MAX_DISCARD_BUFFERS];
	uint32_t discardBufferCount;

	void* delelteBuffers[MAX_DISCARD_BUFFERS];
	uint32_t delelteBufferCount;

	DisplayBuffer()
		: context(),
		  cueHeap(NULL),
		  dcbBuffer(NULL),
		  ccbBuffer(NULL),
		  renderTarget(),
		  hasDepthTarget(false),
		  depthTarget(),
		  state(NULL),
		  currentVB(NULL),
		  currentVBDirty(false),
		  currentVBOffset(0),
		  currentIB(NULL),
		  currentIBDirty(false),
		  freeBufferCount(0),
		  discardBufferCount(0),
		  delelteBufferCount(0)
	{
		memset(freeBuffers, 0, sizeof(freeBuffers));
		memset(discardBuffers, 0, sizeof(discardBuffers));
		memset(delelteBuffers, 0, sizeof(delelteBuffers));		
	}
};

} // namespace Graphics