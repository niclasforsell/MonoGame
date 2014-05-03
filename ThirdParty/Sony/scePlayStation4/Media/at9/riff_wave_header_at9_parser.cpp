/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2013 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "riff_wave_header_at9_parser.h"

using namespace Media;

#define WAVE_FORMAT_EXTENSIBLE (0xFFFE)

static const uint8_t s_subFormat[16] = {
	// 0x47E142D2, 0x36BA, 0x4D8D, 0x88FC61654F8C836C
	0xD2, 0x42, 0xE1, 0x47, 0xBA, 0x36, 0x8D, 0x4D, 0x88, 0xFC, 0x61, 0x65, 0x4F, 0x8C, 0x83, 0x6C
};

int RiffWaveHeaderAt9Parser::parse(const uint8_t *pBuffer, uint32_t bufferSize)
{
	uint32_t readSize = 0;
	uint32_t chunkDataSize;

	uint32_t headerSize = 0;

	assert(pBuffer);

	// clear context
	memset(&m_header, 0, sizeof(m_header));

	// find RIFF-WAVE chunk
	for ( ; ; ) {
		if (bufferSize < readSize + sizeof(RiffWaveHeader)) {
			printf("error: buffer size is too small\n");
			return -1;
		}
		memcpy(&m_header.riffWaveHeader, pBuffer + readSize, sizeof(RiffWaveHeader));
		readSize += sizeof(RiffWaveHeader);

		if (m_header.riffWaveHeader.chunkId != 0x46464952) { // "RIFF"
			printf("error: beginning of header is not \"RIFF\"\n");
			return -1;
		}
		chunkDataSize = m_header.riffWaveHeader.chunkDataSize;
		if (chunkDataSize % 2 == 1) {
			// if chunkDataSize is odd, add padding data length
			chunkDataSize += 1;
		}

		if (m_header.riffWaveHeader.typeId == 0x45564157) { // "WAVE"
			break;
		}

		if (bufferSize < readSize + chunkDataSize - sizeof(m_header.riffWaveHeader.typeId)) {
			printf("error: buffer size is too small\n");
			return -1;
		}
		readSize += chunkDataSize - sizeof(m_header.riffWaveHeader.typeId);
	}

	while (headerSize == 0) {
		ChunkHeader chunkHeader;

		if (bufferSize < readSize + sizeof(ChunkHeader)) {
			printf("error: buffer size is too small\n");
			return -1;
		}
		memcpy(&chunkHeader, pBuffer + readSize, sizeof(ChunkHeader));
		readSize += sizeof(ChunkHeader);

		chunkDataSize = chunkHeader.chunkDataSize;
		if (chunkDataSize % 2 == 1) {
			// if chunkDataSize is odd, add padding data length
			chunkDataSize += 1;
		}
		switch (chunkHeader.chunkId) {
		case 0x20746D66: // "fmt "
			memcpy(&m_header.fmtChunkHeader, &chunkHeader, sizeof(ChunkHeader));

			if (bufferSize < readSize + sizeof(FmtChunk)) {
				printf("error: buffer size is too small\n");
				return -1;
			}
			memcpy(&m_header.fmtChunk, pBuffer + readSize, sizeof(FmtChunk));
			readSize += sizeof(FmtChunk);

			if (m_header.fmtChunk.formatTag != WAVE_FORMAT_EXTENSIBLE) {
				printf("unknown ID: %X\n", m_header.fmtChunk.formatTag);
				return -1;
			}
			if (memcmp(s_subFormat, m_header.fmtChunk.subFormat, sizeof(s_subFormat))) {
				const uint32_t *p = (uint32_t *)m_header.fmtChunk.subFormat;
				printf("unknown codec ID: %08X%08X%08X%08X\n", p[0], p[1], p[3], p[4]);
				return -1;
			}
			// rest is unknown data, so just skip them
			chunkDataSize = (chunkDataSize < sizeof(FmtChunk)) ? 0 : chunkDataSize - sizeof(FmtChunk);
			break;
		case 0x74636166: // "fact"
			memcpy(&m_header.factChunkHeader, &chunkHeader, sizeof(ChunkHeader));

			if (bufferSize < readSize + sizeof(FactChunk)) {
				printf("error: buffer size is too small\n");
				return -1;
			}
			memcpy(&m_header.factChunk, pBuffer + readSize, sizeof(FactChunk));
			readSize += sizeof(FactChunk);

			// rest is unknown data, so just skip them
			chunkDataSize = (chunkDataSize < sizeof(FactChunk)) ? 0 : chunkDataSize - sizeof(FactChunk);
			break;
		case 0x61746164: // "data"
			memcpy(&m_header.dataChunkHeader, &chunkHeader, sizeof(ChunkHeader));

			headerSize = readSize;
			break;
		case 0x6C706D73: // "smpl"
			memcpy(&m_header.smplChunkHeader, &chunkHeader, sizeof(ChunkHeader));

			if (bufferSize < readSize + sizeof(SmplChunk)) {
				printf("error: buffer size is too small\n");
				return -1;
			}
			memcpy(&m_header.smplChunk, pBuffer + readSize, sizeof(SmplChunk));
			readSize += sizeof(SmplChunk);

			// rest is unknown data, so just skip them
			chunkDataSize = (chunkDataSize < sizeof(SmplChunk)) ? 0 : chunkDataSize - sizeof(SmplChunk);
			break;
		default: // unknown chunk
			printf("unknown\n");
			break;
		}
		if (bufferSize < readSize + chunkDataSize) {
			printf("error: buffer size is too small\n");
			return -1;
		}
		readSize += chunkDataSize;
	}

#ifdef DISPLAY_HEADER
	printf("= ATRAC9 Audio header ===============\n");
	printf("RiffWaveHeader.chunkId                    : 0x%X (0x45564157 (\"RIFF\") fixed)\n", m_header.riffWaveHeader.chunkId);
	printf("RiffWaveHeader.chunkDataSize              : %u\n", m_header.riffWaveHeader.chunkDataSize);
	printf("RiffWaveHeader.typeId                     : 0x%X (0x45564157 (\"WAVE\") fixed)\n", m_header.riffWaveHeader.typeId);
	printf("ChunkHeader.chunkId                       : 0x%X (0x20746D66 (\"fmt \") fixed)\n", m_header.fmtChunkHeader.chunkId);
	printf("ChunkHeader.chunkDataSize                 : %u\n", m_header.fmtChunkHeader.chunkDataSize);
	printf("FmtChunk.formatTag                        : 0x%X (0xFFFE fixed)\n", m_header.fmtChunk.formatTag);
	printf("FmtChunk.numChannels                      : %u\n", m_header.fmtChunk.numChannels);
	printf("FmtChunk.samplesPerSec                    : %u Hz\n", m_header.fmtChunk.samplesPerSec);
	printf("FmtChunk.avgBytesPerSec                   : %u kbps\n", m_header.fmtChunk.avgBytesPerSec * 8 / 1000);
	printf("FmtChunk.blockAlign                       : %u\n", m_header.fmtChunk.blockAlign);
	printf("FmtChunk.bitsPerSample                    : %u\n", m_header.fmtChunk.bitsPerSample);
	printf("FmtChunk.cbSize                           : %u\n", m_header.fmtChunk.cbSize);
	printf("FmtChunk.samplesPerBlock                  : %u\n", m_header.fmtChunk.samplesPerBlock);
	printf("FmtChunk.chennelMask                      : 0x%X\n", m_header.fmtChunk.chennelMask);
	printf("FmtChunk.subFormat                        : 0x%X* (47E142D2-36BA-4D8D-88FC61654F8C836C fixed)\n", m_header.fmtChunk.subFormat[0]);
	printf("FmtChunk.versionInfo                      : 0x%X\n", m_header.fmtChunk.versionInfo);
	printf("FmtChunk.configData                       : 0x%X*\n", m_header.fmtChunk.configData[0]);
	printf("FmtChunk.reserved                         : 0x%X\n", m_header.fmtChunk.reserved);
	printf("ChunkHeader.chunkId                       : 0x%X (0x74636166 (\"fact\") fixed)\n", m_header.factChunkHeader.chunkId);
	printf("ChunkHeader.chunkDataSize                 : %u\n", m_header.factChunkHeader.chunkDataSize);
	printf("FactChunk.totalSamples                    : %u\n", m_header.factChunk.totalSamples);
	printf("FactChunk.delaySamplesInputOverlap        : %u\n", m_header.factChunk.delaySamplesInputOverlap);
	printf("FactChunk.delaySamplesInputOverlapEncoder : %u\n", m_header.factChunk.delaySamplesInputOverlapEncoder);
	printf("ChunkHeader.chunkId                       : 0x%X (0x6C706D73 (\"smpl\") fixed)\n", m_header.smplChunkHeader.chunkId);
	printf("ChunkHeader.chunkDataSize                 : %u\n", m_header.smplChunkHeader.chunkDataSize);
	printf("SmplChunk.sampleLoops                     : %u\n", m_header.smplChunk.sampleLoops);
	printf("SmplChunk.SampleLoop.start                : %u\n", m_header.smplChunk.sampleLoop.start);
	printf("SmplChunk.SampleLoop.end                  : %u\n", m_header.smplChunk.sampleLoop.end);
	printf("ChunkHeader.chunkId                       : 0x%X (0x61746164 (\"data\") fixed)\n", m_header.dataChunkHeader.chunkId);
	printf("ChunkHeader.chunkDataSize                 : %u\n", m_header.dataChunkHeader.chunkDataSize);
	printf("=====================================\n");
#endif /* DISPLAY_HEADER */

	// header size
	m_headerSize = headerSize;

	return 0;
}
