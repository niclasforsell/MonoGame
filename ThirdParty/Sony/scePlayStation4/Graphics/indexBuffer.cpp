#include "indexBuffer.h"

#include "graphicsSystem.h"

#include "../allocator.h"
#include <gnm.h>
#include <assert.h>
#include <math.h>

using namespace sce;
using namespace sce::Gnm;
using namespace Graphics;


IndexBuffer::IndexBuffer(IndexElement type, int32_t indexCount)
{
	_indexSize = type == IndexElement_SixteenBits ? Gnm::kIndexSize16 : Gnm::kIndexSize32;
	_indexCount = indexCount;

	_actualSize = _requiredSize = indexCount * (type == IndexElement_SixteenBits ? sizeof(uint16_t) : sizeof(uint32_t));
	_bufferData = mem::allocShared<uint8_t>(_actualSize, Gnm::kAlignmentOfBufferInBytes);

	// TODO: Should we clear the buffer to zeros?  Maybe we
	// should clear it to 0xd34db33f in debug modes to ensure
	// the error is very apparent?
	//memset(_bufferData, 0xd34db33f, _actualSize);
}

IndexBuffer::~IndexBuffer()
{
	mem::freeShared(_bufferData);
}

void IndexBuffer::SetData(GraphicsSystem *system, int32_t offsetInBytes, uint8_t *data, int32_t bytes, bool discard)
{
	// Are we discarding this buffer for another?
	if (discard)
		system->_discardBuffer(this);

	assert(_actualSize >= _requiredSize);
	assert(offsetInBytes + bytes <= _requiredSize);
	memcpy(_bufferData + offsetInBytes, data, bytes);
}
