#pragma once

#include "predecls.h"
#include "graphicsEnums.h"

namespace Graphics {


class CS_API IndexBuffer
{
	friend class GraphicsSystem;

private:

	GraphicsSystem *_system;

	uint8_t *_bufferData;
	uint32_t _requiredSize;
	uint32_t _actualSize;

	uint32_t _indexCount;
	uint32_t _indexSize;

	// No default or copy constructors.
	IndexBuffer() { }
	IndexBuffer( const IndexBuffer & ) { }


public:
	IndexBuffer(GraphicsSystem *system, IndexElement type, int32_t indexCount);
	virtual ~IndexBuffer();

	virtual void SetData(int32_t offsetInBytes, uint8_t *data, int32_t bytes, bool discard);

};

} // namespace Graphics