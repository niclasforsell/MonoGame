#pragma once

#include "predecls.h"
#include "graphicsEnums.h"

namespace Graphics {

class CS_API VertexBuffer
{
	friend class GraphicsSystem;

private:
	GraphicsSystem *_system;

	uint8_t *_bufferData;
	uint32_t _requiredSize;
	uint32_t _actualSize;

	uint32_t _bufferCount;
	uint32_t _stride;
	sce::Gnm::DataFormat *_format;

	// No default or copy constructors.
	VertexBuffer() { }
	VertexBuffer( const VertexBuffer & ) { }

	static sce::Gnm::DataFormat GetFormat(VertexElement element);

public:
	VertexBuffer(GraphicsSystem *system, int32_t *elements, int32_t elementCount, int32_t vertexStride, int32_t vertexCount);
	virtual ~VertexBuffer();

	virtual void SetData(int32_t offsetInBytes, unsigned char *data, int32_t bytes, bool discard);
	virtual void GetData(int32_t offsetInBytes, unsigned char *data, int32_t dataCount, int32_t dataStride, int32_t vertexStride);

};

} // namespace Graphics