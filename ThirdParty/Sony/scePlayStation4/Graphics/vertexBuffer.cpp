#include "VertexBuffer.h"

#include "graphicsSystem.h"

#include "../allocator.h"
#include <gnm.h>
#include <assert.h>
#include <math.h>

using namespace sce;
using namespace sce::Gnm;
using namespace Graphics;


VertexBuffer::VertexBuffer(GraphicsSystem *system, int32_t *elements, int32_t elementCount, int32_t vertexStride, int32_t vertexCount)
{
	assert(system != nullptr);
	assert(elementCount > 0);
	assert(elementCount < 16);
	assert(vertexStride > 0);
	assert(vertexCount > 0);

	_system = system;
	_stride = vertexStride;
	_actualSize = _requiredSize = vertexStride * vertexCount;

	_bufferData = mem::allocShared<uint8_t>(_actualSize, Gnm::kAlignmentOfBufferInBytes);
	_bufferCount = elementCount;

	_format = mem::alloc_array<Gnm::DataFormat>(_bufferCount);
	for (auto i=0; i < _bufferCount; i++)
	{
		auto format = GetFormat((VertexElement)elements[i]);
		_format[i] = format;
	}

	// TODO: Should we clear the buffer to zeros?  Maybe we
	// should clear it to 0xd34db33f in debug modes to ensure
	// the error is very apparent?
	//memset(_bufferData, 0xd34db33f, _actualSize);
}

VertexBuffer::~VertexBuffer()
{
	mem::free(_format);
	_system->_safeDeleteBuffer(_bufferData);
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

void VertexBuffer::SetData(int32_t offsetInBytes, unsigned char *data, int32_t bytes, bool discard)
{
	// Are we discarding this buffer for another?
	if (discard)
		_system->_discardBuffer(this);

	assert(_actualSize >= _requiredSize);
	assert(offsetInBytes + bytes <= _requiredSize);

	memcpy(_bufferData + offsetInBytes, data, bytes);
}

void VertexBuffer::GetData(int32_t offsetInBytes, unsigned char *data, int32_t dataCount, int32_t dataStride, int32_t vertexStride)
{
	assert(offsetInBytes >= 0);
	assert(offsetInBytes < _requiredSize);
	assert(data != nullptr);
	assert(dataStride > 0);
	assert(vertexStride > 0);
	assert(offsetInBytes + (dataCount * dataStride) <= _requiredSize);

	// We can either be copying a solid block of data or we
	// could be copying a single element of the vertex.
	if (dataStride == vertexStride)
		memcpy(data, _bufferData + offsetInBytes, dataStride * dataCount);
	else
	{
		for (auto i=0; i < dataCount; i++)
			memcpy(data + (i * dataStride), _bufferData + offsetInBytes + (i * vertexStride), dataStride);
	}
}
