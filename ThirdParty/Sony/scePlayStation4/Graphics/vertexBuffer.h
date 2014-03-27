#pragma once

#include "predecls.h"

namespace Graphics {


enum VertexElement
{
	// NOTE: This should match up with the MonoGame 
	// VertexElementFormat enum for easy interop!

	VertexElement_Single,
    VertexElement_Vector2,
    VertexElement_Vector3,
    VertexElement_Vector4,
    VertexElement_Color,
    VertexElement_Byte4,
    VertexElement_Short2,
    VertexElement_Short4,
    VertexElement_NormalizedShort2,
    VertexElement_NormalizedShort4,
    VertexElement_HalfVector2,
    VertexElement_HalfVector4
};


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

	static sce::Gnm::DataFormat GetFormat(VertexElement element);

public:
	VertexBuffer(VertexElement *elements, uint32_t elementCount, uint32_t vertexStride, uint32_t vertexCount);
	virtual ~VertexBuffer();

	virtual void SetData(uint32_t offsetInBytes, unsigned char *data, uint32_t bytes);

};

} // namespace Graphics