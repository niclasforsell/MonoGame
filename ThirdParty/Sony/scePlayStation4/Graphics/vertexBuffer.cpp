#include "VertexBuffer.h"

#include "../allocator.h"
#include <gnm.h>
#include <assert.h>
#include <math.h>

using namespace sce;
using namespace sce::Gnm;
using namespace Graphics;


VertexBuffer::VertexBuffer(uint32_t vertexStride, uint32_t vertexCount)
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

void VertexBuffer::SetData(uint32_t offset, unsigned char *data, uint32_t bytes)
{
	memcpy((unsigned char*)_bufferData + offset, data, bytes);
}
