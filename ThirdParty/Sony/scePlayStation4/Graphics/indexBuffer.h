#pragma once

#include "predecls.h"
#include "graphicsEnums.h"

namespace Graphics {


class __declspec(dllexport) IndexBuffer
{
	friend class GraphicsSystem;

private:

	void *_indexData;
	uint32_t _indexCount;
	int32_t _indexSize;

	// No default or copy constructors.
	IndexBuffer() { }
	IndexBuffer( const IndexBuffer & ) { }


public:
	IndexBuffer(IndexElement type, uint32_t indexCount);
	virtual ~IndexBuffer();

	virtual void SetData(uint32_t offsetInBytes, unsigned char *data, uint32_t bytes);

};

} // namespace Graphics