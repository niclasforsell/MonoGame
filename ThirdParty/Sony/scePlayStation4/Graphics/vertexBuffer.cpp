#include "VertexBuffer.h"

#include "../allocator.h"
#include <gnm.h>
#include <assert.h>
#include <math.h>

using namespace sce;
using namespace sce::Gnm;
using namespace Graphics;


VertexBuffer::VertexBuffer(VertexElement *elements, uint32_t elementCount, uint32_t vertexStride, uint32_t vertexCount)
{
	auto sizeInBytes = vertexStride * vertexCount;
	_bufferData = Allocator::Get()->allocate(sizeInBytes, Gnm::kAlignmentOfBufferInBytes, SCE_KERNEL_WC_GARLIC);

	//_buffers[0].initAsVertexBuffer(_bufferData + 0, Gnm::kDataFormatR32G32B32Float, vertexStride, vertCount);
	//_buffers[1].initAsVertexBuffer(_bufferData + 12, Gnm::kDataFormatR8G8B8A8Unorm, vertexStride, vertCount);
	//_buffers[2].initAsVertexBuffer(_bufferData + 16, Gnm::kDataFormatR32G32Float, vertexStride, vertCount);
}

VertexBuffer::~VertexBuffer()
{
	Allocator::Get()->release(_bufferData);
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

void VertexBuffer::SetData(uint32_t offsetInBytes, unsigned char *data, uint32_t bytes)
{
	memcpy((unsigned char*)_bufferData + offsetInBytes, data, bytes);
}
