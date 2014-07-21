#include "pngReader.h"
#include "../allocator.h"
#include <memory.h>
#include <libsysmodule.h>

using namespace Graphics;

PngReader::PngReader()
: _decodeBuffer(nullptr),
_imageBuffer(nullptr),
_imageBufferSize(0),
_handle(nullptr)
{
	memset(&_imageInfo, 0, sizeof(ScePngDecImageInfo));
}

PngReader::~PngReader()
{
	Cleanup();

	auto ret = scePngDecDelete(_handle);
	
	if (_decodeBuffer != nullptr)
	{
		mem::free(_decodeBuffer);
		_decodeBuffer = nullptr;
	}

	_imageBufferSize = 0;
}

PngReaderResult PngReader::Initialize(int maxImageWidth)
{
	if (sceSysmoduleIsLoaded(SCE_SYSMODULE_PNG_DEC) != SCE_OK)
	{
		if (sceSysmoduleLoadModule(SCE_SYSMODULE_PNG_DEC) != SCE_OK)
			return PngReaderResult::Fatal;
	}

	ScePngDecCreateParam createParam;
	memset(&createParam, 0, sizeof(ScePngDecCreateParam));
	createParam.thisSize = sizeof(ScePngDecCreateParam);
	createParam.attribute = SCE_PNG_DEC_ATTRIBUTE_NONE;
	createParam.maxImageWidth = maxImageWidth;
	auto decoderSize = scePngDecQueryMemorySize(&createParam);
	if (decoderSize < 0)
		return (PngReaderResult)decoderSize;

	_decodeBuffer = mem::alloc(decoderSize);
	memset(_decodeBuffer, 0, decoderSize);
	auto ret = scePngDecCreate(&createParam, _decodeBuffer, decoderSize, &_handle);

	return (PngReaderResult)ret;
}

PngReaderResult PngReader::LoadCompressed(const uint8_t* compressed, int size)
{
	Cleanup();

	ScePngDecParseParam parseParam;
	memset(&parseParam, 0, sizeof(ScePngDecParseParam));
	parseParam.pngMemAddr = compressed;
	parseParam.pngMemSize = size;

	auto ret = scePngDecParseHeader(&parseParam, &_imageInfo);
	if (ret < 0)
		return (PngReaderResult)ret;

	_imageBufferSize = _imageInfo.imageWidth * _imageInfo.imageHeight * 4;
	_imageBuffer = mem::alloc(_imageBufferSize);
	memset(_imageBuffer, 0, _imageBufferSize);

	ScePngDecDecodeParam decodeParam;
	memset(&decodeParam, 0, sizeof(ScePngDecDecodeParam));
	decodeParam.pngMemAddr = compressed;
	decodeParam.pngMemSize = size;
	decodeParam.imageMemAddr = _imageBuffer;
	decodeParam.imageMemSize = _imageBufferSize;
	decodeParam.pixelFormat = SCE_PNG_DEC_PIXEL_FORMAT_R8G8B8A8;
	decodeParam.alphaValue = 255;
	decodeParam.imagePitch = 0;
	int32_t pixels = scePngDecDecode(_handle, &decodeParam, NULL);
	if (pixels < 0)
		return (PngReaderResult)pixels;

	return PngReaderResult::OK;
}

int PngReader::GetOutputSize() const
{
	return _imageBufferSize;
}

int PngReader::GetWidth() const
{
	return _imageInfo.imageWidth;
}

int PngReader::GetHeight() const
{
	return _imageInfo.imageHeight;
}

PngReaderResult PngReader::OutputUncompressed(uint8_t* buffer, int size)
{
	if (size != GetOutputSize())
		return PngReaderResult::InvalidSize;

	memcpy(buffer, _imageBuffer, size);

	return PngReaderResult::OK;
}

void PngReader::Cleanup()
{
	if (_imageBuffer != nullptr)
	{
		mem::free(_imageBuffer);
		_imageBuffer = nullptr;
	}

	_imageBufferSize = 0;
}
