#include "Texture.h"

#include "../allocator.h"
#include <gnm.h>
#include <assert.h>

using namespace Graphics;

Texture::Texture(sce::Gnm::Texture *texture)
{
	_texture = texture;
}

Texture::~Texture()
{
	Allocator::Get()->release(_texture->getBaseAddress());
	delete _texture;
}

uint32_t Texture::getWidth()
{ 
	return _texture->getWidth();
}

uint32_t Texture::getHeight() 
{ 
	return _texture->getHeight();
}

void Texture::SetData(unsigned char *data, uint32_t bytes)
{
	auto width = _texture->getWidth();
	auto height = _texture->getHeight();
	auto pixelBytes = _texture->getDataFormat().getBytesPerElement();

	// We expect this call to set the entire 
	// buffer at this point.
	assert((width * height * pixelBytes) == bytes);

	auto baseAddr = (unsigned char*)_texture->getBaseAddress();
	auto pitch = _texture->getPitch();

	if (pitch == width)
		memcpy(baseAddr, data, bytes);
	else
	{
		for (auto h=0; h < height; h++)
		{
			memcpy(baseAddr, data, width * pixelBytes);
			data += width * pixelBytes;
			baseAddr += pitch * pixelBytes;
		}
	}
}
