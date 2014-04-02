/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2012 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#ifndef _RIFF_WAVE_HEADER_AT9_PARSER_H
#define _RIFF_WAVE_HEADER_AT9_PARSER_H

#include <stdint.h>

namespace Media {

typedef struct RiffWaveHeader {
	uint32_t chunkId;
	uint32_t chunkDataSize;
	uint32_t typeId;
} RiffWaveHeader;

typedef struct ChunkHeader {
	uint32_t chunkId;
	uint32_t chunkDataSize;
} ChunkHeader;

typedef struct FmtChunk {
	uint16_t formatTag; // format ID
	uint16_t numChannels; // number of channels, 1 = mono, 2 = stereo
	uint32_t samplesPerSec; // sampling rate
	uint32_t avgBytesPerSec; // average bite rate
	uint16_t blockAlign; // audio block size, ((mono) 1 = 8bit, 2 = 16bit), ((stereo) 2 = 8bit, 4 = 16bit)
	uint16_t bitsPerSample; // quantization bit rate, 8, 12, 16
	uint16_t cbSize; // extension size, 34
	uint16_t samplesPerBlock; // number of output samples of audio block
	uint32_t chennelMask; // mapping of channels to spatial location
	uint8_t subFormat[16]; // codec ID
	uint32_t versionInfo; // version information, 0
	uint8_t configData[4]; // ATRAC9 setting information
	uint32_t reserved; // reserved, 0
} FmtChunk;

typedef struct FactChunk {
	uint32_t totalSamples; // total samples per chennel
	uint32_t delaySamplesInputOverlap; // samples of input and overlap delay
	uint32_t delaySamplesInputOverlapEncoder; // samples of input and overlap and encoder delay
} FactChunk;

typedef struct SampleLoop {
	uint32_t identifier;
	uint32_t type;
	uint32_t start;
	uint32_t end;
	uint32_t fraction;
	uint32_t playCount;
} SampleLoop;

typedef struct SmplChunk {
	uint32_t manufacturer;
	uint32_t product;
	uint32_t samplePeriod;
	uint32_t midiUnityNote;
	uint32_t midiPitchFraction;
	uint32_t smpteFormat;
	uint32_t smpteOffset;
	uint32_t sampleLoops;
	uint32_t samplerData;
	SampleLoop sampleLoop;
} SmplChunk;

typedef struct RiffWaveHeaderAt9 {
	RiffWaveHeader riffWaveHeader;
	ChunkHeader fmtChunkHeader;
	FmtChunk fmtChunk;
	ChunkHeader factChunkHeader;
	FactChunk factChunk;
	ChunkHeader smplChunkHeader;
	SmplChunk smplChunk;
	ChunkHeader dataChunkHeader;
} RiffWaveHeaderAt9;

class RiffWaveHeaderAt9Parser
{
public:
	RiffWaveHeaderAt9Parser(void) : m_headerSize(0) {}
	~RiffWaveHeaderAt9Parser(void) {}
	const RiffWaveHeaderAt9 &header() { return m_header; }
	uint32_t headerSize(void) { return m_headerSize; }
	int parse(const uint8_t *pBuffer, uint32_t bufferSize);
private:
	RiffWaveHeaderAt9 m_header;
	uint32_t m_headerSize;
};

} // namespace Media

#endif /* _RIFF_WAVE_HEADER_AT9_PARSER_H */
