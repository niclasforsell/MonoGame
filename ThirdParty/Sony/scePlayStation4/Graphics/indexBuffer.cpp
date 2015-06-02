#include "indexBuffer.h"

#include "graphicsSystem.h"

#include "../allocator.h"
#include <gnm.h>
#include <assert.h>
#include <math.h>

using namespace sce;
using namespace sce::Gnm;
using namespace Graphics;


IndexBuffer::IndexBuffer(GraphicsSystem *system, IndexElement type, int32_t indexCount)
{
	_system = system;

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
	_system->_safeDeleteBuffer(_bufferData);
}

void IndexBuffer::SetData(int32_t offsetInBytes, uint8_t *data, int32_t bytes, bool discard)
{
	// Check inputs.
	assert(offsetInBytes >= 0);
	assert(offsetInBytes < _requiredSize);
	assert(data != nullptr);
	assert(bytes > 0);
	assert(bytes <= _requiredSize);
	assert(offsetInBytes + bytes <= _requiredSize);

	// Are we discarding this buffer for another?
	if (discard)
		_system->_discardBuffer(this);

	// Make sure the new buffer is big enough.
	assert(_actualSize >= _requiredSize);

	auto ebuffer = _bufferData + _requiredSize;
	auto sdest = _bufferData + offsetInBytes;
	auto edest = sdest + bytes;
	assert(edest <= ebuffer);

	memcpy(sdest, data, bytes);
}

void IndexBuffer::GetData(int32_t offsetInBytes, uint8_t *data, int32_t bytes)
{
	assert(offsetInBytes >= 0);
	assert(offsetInBytes < _requiredSize);
	assert(data != nullptr);
	assert(bytes > 0);
	assert(bytes <= _requiredSize);
	assert(offsetInBytes + bytes <= _requiredSize);

	memcpy(data, _bufferData + offsetInBytes, bytes);
}
