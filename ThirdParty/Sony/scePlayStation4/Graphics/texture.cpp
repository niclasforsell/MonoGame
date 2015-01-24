#include "Texture.h"
#include "renderTarget.h"

#include "graphicsHelpers.h"
#include "../allocator.h"

#include <sdk_version.h>
#include <gnm.h>
#include <assert.h>
#include <math.h>

using namespace Graphics;

Texture* Texture::Create2D(TextureFormat format, int32_t width, int32_t height, int32_t mips)
{
	assert(mips >= 1);

	auto texture = new sce::Gnm::Texture();
	auto result = new Texture();
	result->_texture = texture;


	// BCn compressed textures come through the pipeline as
	// tiled data, otherwise use linear for flexibility.
	Gnm::TileMode tileMode;
	switch(format)
	{
	case TextureFormat::TextureFormat_PlayStation4_BC1Unorm:
	case TextureFormat::TextureFormat_PlayStation4_BC2Unorm:
	case TextureFormat::TextureFormat_PlayStation4_BC3Unorm:
		tileMode = Gnm::kTileModeThin_1dThin;
		break;

	default:
		tileMode = Gnm::kTileModeDisplay_LinearAligned;
	}

	auto textureSizeAlign = texture->initAs2d(
		width, height, mips,
		ToDataFormat(format),
		tileMode,
#if SCE_ORBIS_SDK_VERSION >= 0x02000071u // SDK Version 2.0
		Gnm::kNumFragments1);
#else
		Gnm::kNumSamples1);
#endif

	// Allocate the texture data using the alignment returned by initAs2d
	void *textureData = mem::allocShared(textureSizeAlign);
	texture->setBaseAddress(textureData);

	return result;
}

Texture* Texture::Create3D(TextureFormat format, int32_t width, int32_t height, int32_t depth, int32_t mips)
{
	assert(mips >= 1);

	auto texture = new sce::Gnm::Texture();
	auto result = new Texture();
	result->_texture = texture;

	// BCn compressed textures come through the pipeline as
	// tiled data, otherwise use linear for flexibility.
	Gnm::TileMode tileMode;
	switch(format)
	{
	case TextureFormat::TextureFormat_PlayStation4_BC1Unorm:
	case TextureFormat::TextureFormat_PlayStation4_BC2Unorm:
	case TextureFormat::TextureFormat_PlayStation4_BC3Unorm:
		tileMode = Gnm::kTileModeThin_1dThin;
		break;

	default:
		tileMode = Gnm::kTileModeDisplay_LinearAligned;
	}

	auto textureSizeAlign = texture->initAs3d(
		width, height, depth, mips,
		ToDataFormat(format),
		tileMode);

	// Allocate the texture data using the alignment returned by initAs3d
	void *textureData = mem::allocShared(textureSizeAlign);
	texture->setBaseAddress(textureData);

	return result;
}

Texture* Texture::CreateCube(TextureFormat format, int32_t width, int32_t height, int32_t mips)
{
	assert(mips >= 1);

	auto texture = new sce::Gnm::Texture();
	auto result = new Texture();
	result->_texture = texture;

	// BCn compressed textures come through the pipeline as
	// tiled data, otherwise use linear for flexibility.
	Gnm::TileMode tileMode;
	switch(format)
	{
	case TextureFormat::TextureFormat_PlayStation4_BC1Unorm:
	case TextureFormat::TextureFormat_PlayStation4_BC2Unorm:
	case TextureFormat::TextureFormat_PlayStation4_BC3Unorm:
		tileMode = Gnm::kTileModeThin_1dThin;
		break;

	default:
		tileMode = Gnm::kTileModeDisplay_LinearAligned;
	}

	auto textureSizeAlign = texture->initAsCubemap(
		width, height, mips,
		ToDataFormat(format),
		tileMode);

	// Allocate the texture data using the alignment returned by initAsCubemap
	void *textureData = mem::allocShared(textureSizeAlign);
	texture->setBaseAddress(textureData);

	return result;
}

Texture::Texture()
{ 
	_isTarget = false; 
	_ownsTexture = true; 
}

Texture::Texture( const Texture & )
{
}

Texture::~Texture()
{
	if (!_ownsTexture)
		return;

	mem::freeShared(_texture->getBaseAddress());
	delete _texture;
}

void Texture::SetData(uint32_t mipLevel, uint8_t* data, uint32_t offset, uint32_t length)
{
	auto width = MAX(1, _texture->getWidth() >> mipLevel);
	auto height = MAX(1, _texture->getHeight() >> mipLevel);
	auto depth = MAX(1, _texture->getDepth() >> mipLevel);
	auto pixelBits = _texture->getDataFormat().getBitsPerElement();

	uint64_t levelOffset, levelSize;
	GpuAddress::computeTextureSurfaceOffsetAndSize(&levelOffset, &levelSize, _texture, mipLevel, 0);

	auto baseAddr = (unsigned char*)_texture->getBaseAddress();
	baseAddr += levelOffset;

	if (offset == 0 && length == levelSize)
		memcpy(baseAddr, data, length);
	else
	{
		GpuAddress::TilingParameters tile;
		tile.initFromTexture(_texture, mipLevel, 0);
		GpuAddress::SurfaceInfo info;
		GpuAddress::computeSurfaceInfo(&info, &tile);

		auto pitch = info.m_pitch;
		auto dataPitchBytes = (width * pixelBits) / 8;
		auto texPitchBytes = (pitch * pixelBits) / 8;

		for (auto h=0; h < height; h++)
		{
			memcpy(baseAddr, data, MIN(dataPitchBytes, length));

			// Have we copied all the data we got?
			length -= dataPitchBytes;
			if (length <= 0)
				break;

			data += dataPitchBytes;
			baseAddr += texPitchBytes;
		}
	}
}

void Texture::GetData(uint32_t mipLevel, uint8_t* data, uint32_t offset, uint32_t length)
{
	auto width = MAX(1, _texture->getWidth() >> mipLevel);
	auto height = MAX(1, _texture->getHeight() >> mipLevel);
	auto depth = MAX(1, _texture->getDepth() >> mipLevel);
	auto pixelBits = _texture->getDataFormat().getBitsPerElement();

	uint64_t levelOffset, levelSize;
	unsigned char* baseAddr;

	if (_isTarget)	
	{
		auto target = (RenderTarget*)this;
		baseAddr = target->GetDataDetiled(&levelOffset, &levelSize, mipLevel);
	}
	else
	{
		GpuAddress::computeTextureSurfaceOffsetAndSize(&levelOffset, &levelSize, _texture, mipLevel, 0);
		baseAddr = (unsigned char*)_texture->getBaseAddress();
		baseAddr += levelOffset;
	}

	if (offset == 0 && length == levelSize)
		memcpy(data, baseAddr, length);
	else
	{
		GpuAddress::TilingParameters tile;
		tile.initFromTexture(_texture, mipLevel, 0);
		GpuAddress::SurfaceInfo info;
		GpuAddress::computeSurfaceInfo(&info, &tile);

		auto pitch = info.m_pitch;
		auto texPitchBytes = (pitch * pixelBits) / 8;
		auto dataPitchBytes = (width * pixelBits) / 8;

		for (auto h=0; h < height; h++)
		{
			memcpy(data, baseAddr, MIN(dataPitchBytes, length));

			// Have we filled the data buffer?
			length -= dataPitchBytes;
			if (length <= 0)
				break;

			data += dataPitchBytes;
			baseAddr += texPitchBytes;
		}
	}
}

sce::Gnm::Texture* Texture::GetInternalData() const
{
	return _texture;
}

