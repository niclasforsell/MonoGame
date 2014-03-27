#include "Texture.h"

#include "../allocator.h"
#include <gnm.h>
#include <assert.h>
#include <math.h>

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

void Texture::SetData(uint32_t level, unsigned char *data, uint32_t bytes)
{
	auto width = _texture->getWidth();
	auto height = _texture->getHeight();
	auto pixelBytes = _texture->getDataFormat().getBytesPerElement();
	auto baseAddr = (unsigned char*)_texture->getBaseAddress();
	auto pitch = _texture->getPitch();
	auto levelZeroSize = pitch * height * pixelBytes;

	while (level > 1)
	{
		baseAddr += levelZeroSize;
		levelZeroSize /= 4;
		--level;

		width = MAX(1, width << 1);
		height = MAX(1, height << 1);
		pitch = MAX(1, pitch << 1);
	}

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
