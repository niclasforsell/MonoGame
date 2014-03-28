#include "indexBuffer.h"

#include "../allocator.h"
#include <gnm.h>
#include <assert.h>
#include <math.h>

using namespace sce;
using namespace sce::Gnm;
using namespace Graphics;


IndexBuffer::IndexBuffer(IndexElement type, uint32_t indexCount)
{
	_indexSize = type == IndexElement_SixteenBits ? Gnm::kIndexSize16 : Gnm::kIndexSize32;

	_indexCount = indexCount;
	auto sizeInBytes = indexCount * (type == IndexElement_SixteenBits ? sizeof(uint16_t) : sizeof(uint32_t));
	_indexData = Allocator::Get()->allocate(sizeInBytes, Gnm::kAlignmentOfBufferInBytes, SCE_KERNEL_WC_GARLIC);

	// It is worth initializing it here for consistant
	// behavior when accidentally unset.
	memset(_indexData, 0, sizeInBytes);
}

IndexBuffer::~IndexBuffer()
{
	Allocator::Get()->release(_indexData);
}

void IndexBuffer::SetData(uint32_t offsetInBytes, unsigned char *data, uint32_t bytes)
{
	memcpy((unsigned char*)_indexData + offsetInBytes, data, bytes);
}
