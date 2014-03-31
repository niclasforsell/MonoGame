#pragma once

#include "predecls.h"
#include "graphicsEnums.h"

namespace Graphics {

class __declspec(dllexport) VertexBuffer
{
	friend class GraphicsSystem;

private:

	uint8_t *_bufferData;
	uint32_t _bufferSize;

	uint32_t _bufferCount;
	sce::Gnm::Buffer *_buffers;

	// No default or copy constructors.
	VertexBuffer() { }
	VertexBuffer( const VertexBuffer & ) { }

	static sce::Gnm::DataFormat GetFormat(VertexElement element);

public:
	VertexBuffer(int32_t *elements, int32_t elementCount, int32_t vertexStride, int32_t vertexCount);
	virtual ~VertexBuffer();

	virtual void SetData(GraphicsSystem *system, int32_t offsetInBytes, unsigned char *data, int32_t bytes, bool discard);

};

} // namespace Graphics