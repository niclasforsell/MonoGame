#pragma once

#include "predecls.h"
#include <png_dec.h>

namespace Graphics {

enum class PngReaderResult
{
	OK = 0,

	InvalidAddr = SCE_PNG_DEC_ERROR_INVALID_ADDR,
	InvalidSize = SCE_PNG_DEC_ERROR_INVALID_SIZE,
	InvalidParam = SCE_PNG_DEC_ERROR_INVALID_PARAM,
	InvalidHandle = SCE_PNG_DEC_ERROR_INVALID_HANDLE,
	InvalidWorkMemory = SCE_PNG_DEC_ERROR_INVALID_WORK_MEMORY,
	InvalidData = SCE_PNG_DEC_ERROR_INVALID_DATA,
	UnsupportedData = SCE_PNG_DEC_ERROR_UNSUPPORT_DATA,
	DecodeError = SCE_PNG_DEC_ERROR_DECODE_ERROR,
	Fatal = SCE_PNG_DEC_ERROR_FATAL,
};

class CS_API PngReader
{
	CS_IGNORE void* _decodeBuffer;
	CS_IGNORE void* _imageBuffer;
	CS_IGNORE int _imageBufferSize;

	CS_IGNORE ScePngDecHandle _handle;
	CS_IGNORE ScePngDecImageInfo _imageInfo;

public:
	PngReader();
	~PngReader();

	PngReaderResult Initialize(int maxImageWidth = 4096);

	PngReaderResult LoadCompressed(const uint8_t* compressed, int size);

	int GetOutputSize() const;
	int GetWidth() const;
	int GetHeight() const;

	PngReaderResult OutputUncompressed(uint8_t* buffer, int size);

	void Cleanup();
};

}
