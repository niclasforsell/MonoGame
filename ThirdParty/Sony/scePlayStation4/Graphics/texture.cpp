#include "Texture.h"
#include "renderTarget.h"

#include "graphicsHelpers.h"
#include "../allocator.h"
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

	auto textureSizeAlign = texture->initAs2d(
		width, height, mips,
		ToDataFormat(format),
		Gnm::kTileModeDisplay_LinearAligned,
		Gnm::kNumSamples1);

	// Allocate the texture data using the alignment returned by initAs2d
	void *textureData = Allocator::Get()->allocate(textureSizeAlign, SCE_KERNEL_WC_GARLIC);
	texture->setBaseAddress(textureData);

	return result;
}

Texture* Texture::Create3D(TextureFormat format, int32_t width, int32_t height, int32_t depth, int32_t mips)
{
	assert(mips >= 1);

	auto texture = new sce::Gnm::Texture();
	auto result = new Texture();
	result->_texture = texture;

	auto textureSizeAlign = texture->initAs3d(
		width, height, depth, mips,
		ToDataFormat(format),
		Gnm::kTileModeDisplay_LinearAligned);

	// Allocate the texture data using the alignment returned by initAs3d
	void *textureData = Allocator::Get()->allocate(textureSizeAlign, SCE_KERNEL_WC_GARLIC);
	texture->setBaseAddress(textureData);

	return result;
}

Texture* Texture::CreateCube(TextureFormat format, int32_t width, int32_t height, int32_t mips)
{
	assert(mips >= 1);

	auto texture = new sce::Gnm::Texture();
	auto result = new Texture();
	result->_texture = texture;

	auto textureSizeAlign = texture->initAsCubemap(
		width, height, mips,
		ToDataFormat(format),
		Gnm::kTileModeDisplay_LinearAligned);

	// Allocate the texture data using the alignment returned by initAsCubemap
	void *textureData = Allocator::Get()->allocate(textureSizeAlign, SCE_KERNEL_WC_GARLIC);
	texture->setBaseAddress(textureData);

	return result;
}

Texture::~Texture()
{
	Allocator::Get()->release(_texture->getBaseAddress());
	delete _texture;
}

void Texture::SetData(uint32_t level, uint8_t* data, uint32_t offset, uint32_t length)
{
	auto width = _texture->getWidth();
	auto height = _texture->getHeight();
	auto depth = _texture->getDepth();
	auto pixelBytes = _texture->getDataFormat().getBytesPerElement();
	auto baseAddr = (unsigned char*)_texture->getBaseAddress();
	auto pitch = _texture->getPitch();
	auto levelZeroSize = pitch * height * depth * pixelBytes;

	baseAddr += offset;

	while (level > 0)
	{
		baseAddr += levelZeroSize;
		levelZeroSize /= 4;
		--level;

		width = MAX(1, width >> 1);
		height = MAX(1, height >> 1);
		pitch = MAX(1, pitch >> 1);
	}

	if (pitch == width)
		memcpy(baseAddr, data, length);
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

void Texture::GetData(uint32_t level, uint8_t* data, uint32_t offset, uint32_t length)
{
	auto height = _texture->getHeight();
    auto depth = _texture->getDepth();
	auto pixelBytes = _texture->getDataFormat().getBytesPerElement();
	auto baseAddr = (unsigned char*)_texture->getBaseAddress();
	auto pitch = _texture->getPitch();
	auto levelZeroSize = pitch * height * depth * pixelBytes;

	baseAddr += offset;

	while (level > 0)
	{
		baseAddr += levelZeroSize;
		levelZeroSize /= 4;
		--level;
	}

	memcpy(data, baseAddr, length);
}

/*
Texture* GraphicsSystem::CreateTextureFromPng(unsigned char *data, uint32_t bytes)
{
	ScePngDecParseParam parseParam;
	parseParam.pngMemAddr = data;
	parseParam.pngMemSize = bytes;
	parseParam.reserved0 = 0;

	ScePngDecImageInfo imageInfo;
	int ret = scePngDecParseHeader(&parseParam, &imageInfo);
	if (ret < 0) 
	{
		printf("Error: scePngDecParseHeader(), ret 0x%08x\n", ret);
		return NULL;
	}

	Texture* texture = CreateTexture(imageInfo.imageWidth, imageInfo.imageHeight, 1);
	if (texture == NULL)
		return NULL;

	ScePngDecCreateParam createParam;
	createParam.thisSize = sizeof(createParam);
	createParam.attribute = imageInfo.bitDepth >> 4;
	createParam.maxImageWidth = imageInfo.imageWidth;
	int memorySize = scePngDecQueryMemorySize(&createParam);
	if (memorySize < 0) 
	{
		printf("Error: scePngDecQueryMemorySize(), ret 0x%08x\n", memorySize);
		return NULL;
	}

	// allocate memory for PNG decoder
	// create PNG decoder
	void *decoderBuffer = new char[memorySize];	
	ScePngDecHandle	handle;
	ret = scePngDecCreate(&createParam, decoderBuffer, memorySize, &handle);
	if (ret < 0) 
	{
		printf("Error: scePngDecCreate(), ret 0x%08x\n", ret);
		return NULL;
	}

	//void *image = texture->_texture.
	
	// decode PNG image
	ScePngDecDecodeParam decodeParam;
	decodeParam.pngMemAddr	= data;
	decodeParam.pngMemSize	= bytes;
	decodeParam.imageMemAddr = texture->_texture->getBaseAddress();
	decodeParam.imageMemSize = imageInfo.imageWidth * imageInfo.imageHeight * 4;
	decodeParam.imagePitch	= 0;
	decodeParam.pixelFormat	= SCE_PNG_DEC_PIXEL_FORMAT_R8G8B8A8;
	decodeParam.alphaValue	= 255;
	ret = scePngDecDecode(handle, &decodeParam, NULL);
	if (ret < 0) 
	{
		printf("Error: scePngDecDecode(), ret 0x%08x\n", ret);
		scePngDecDelete(handle);
		return NULL;
	}

	// Cleanup
	scePngDecDelete(handle);
	delete [] decoderBuffer;

	return texture;
}
*/