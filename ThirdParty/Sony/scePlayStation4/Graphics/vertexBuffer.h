#pragma once

#include "predecls.h"

namespace Graphics {

class __declspec(dllexport) VertexBuffer
{
	friend class GraphicsSystem;

private:

	void *_bufferData;
	uint32_t _bufferCount;
	sce::Gnm::Buffer *_buffers;

	// No default or copy constructors.
	VertexBuffer() { }
	VertexBuffer( const VertexBuffer & ) { }

public:
	VertexBuffer(uint32_t vertexStride, uint32_t vertexCount);
	virtual ~VertexBuffer();

	virtual void SetData(uint32_t offset, unsigned char *data, uint32_t bytes);

};

} // namespace Graphics