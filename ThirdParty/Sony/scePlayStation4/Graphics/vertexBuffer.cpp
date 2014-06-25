#include "VertexBuffer.h"

#include "graphicsSystem.h"

#include "../allocator.h"
#include <gnm.h>
#include <assert.h>
#include <math.h>

using namespace sce;
using namespace sce::Gnm;
using namespace Graphics;


VertexBuffer::VertexBuffer(int32_t *elements, int32_t elementCount, int32_t vertexStride, int32_t vertexCount)
{
	_actualSize = _requiredSize = vertexStride * vertexCount;
	_bufferData = mem::allocShared<uint8_t>(_actualSize, Gnm::kAlignmentOfBufferInBytes);

	_bufferCount = elementCount;
	_buffers = mem::alloc_array<Gnm::Buffer>(_bufferCount);
	
	auto offset = 0;
	for (auto i=0; i < _bufferCount; i++)
	{
		auto format = GetFormat((VertexElement)elements[i]);
		_buffers[i].initAsVertexBuffer((uint8_t*)_bufferData + offset, format, vertexStride, vertexCount);
		_buffers[i].setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
		offset += format.getBytesPerElement();
	}

	// TODO: Should we clear the buffer to zeros?  Maybe we
	// should clear it to 0xd34db33f in debug modes to ensure
	// the error is very apparent?
	//memset(_bufferData, 0xd34db33f, _actualSize);
}

VertexBuffer::~VertexBuffer()
{
	mem::free(_buffers);
	mem::freeShared(_bufferData);
}

sce::Gnm::DataFormat VertexBuffer::GetFormat(VertexElement element)
{
	switch (element)
	{
	default:
	case VertexElement_Single:
		return Gnm::kDataFormatR32Float;
	case VertexElement_Vector2:
		return Gnm::kDataFormatR32G32Float;
	case VertexElement_Vector3:
		return Gnm::kDataFormatR32G32B32Float;
	case VertexElement_Vector4:
		return Gnm::kDataFormatR32G32B32A32Float;
	case VertexElement_Color:
		return Gnm::kDataFormatR8G8B8A8Unorm;
	case VertexElement_Byte4:
		return Gnm::kDataFormatR8G8B8A8Uint;
	case VertexElement_Short2:
		return Gnm::kDataFormatR16G16Sint;
	case VertexElement_Short4:
		return Gnm::kDataFormatR16G16B16A16Sint;
	case VertexElement_NormalizedShort2:
		return Gnm::kDataFormatR16G16Snorm;
	case VertexElement_NormalizedShort4:
		return Gnm::kDataFormatR16G16B16A16Snorm;
	case VertexElement_HalfVector2:
		return Gnm::kDataFormatR16G16Float;
	case VertexElement_HalfVector4:
		return Gnm::kDataFormatR16G16B16A16Float;
	};
}

void VertexBuffer::SetData(GraphicsSystem *system, int32_t offsetInBytes, unsigned char *data, int32_t bytes, bool discard)
{
	// Are we discarding this buffer for another?
	if (discard)
		system->_discardBuffer(this);

	assert(_actualSize >= _requiredSize);
	assert(offsetInBytes + bytes <= _requiredSize);

	memcpy(_bufferData + offsetInBytes, data, bytes);
}
