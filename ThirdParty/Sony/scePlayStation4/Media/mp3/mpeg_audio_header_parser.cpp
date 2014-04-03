/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2012 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "mpeg_audio_header_parser.h"

using namespace Media;

#define MPEG_AUDIO_HEADER_SIZE (4)

int MpegAudioHeaderParser::parse(InputStream *input)
{
#ifdef DISPLAY_HEADER
	const char *version[4] = {
		"2.5",
		"X",
		"2",
		"1",
	};
	const uint32_t layer[4] = {
		0, 3, 2, 1
	};
#endif /* DISPLAY_HEADER */
	const uint32_t bitRate[4][16] = {
		{0,  8, 16, 24, 32, 40, 48, 56,  64,  80,  96, 112, 128, 144, 160}, // MPEG2.5
		{0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,   0,   0,   0,   0}, // reserved
		{0,  8, 16, 24, 32, 40, 48, 56,  64,  80,  96, 112, 128, 144, 160}, // MPEG2
		{0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320}, // MPEG1
	};
	const uint32_t samplingFrequency[4][3] = {
		{11025, 12000,  8000}, // MPEG2.5
		{    0,     0,     0}, // reserved
		{22050, 24000, 16000}, // MPEG2
		{44100, 48000, 32000}, // MPEG1
	};
	const uint32_t numChannels[4] = {
		2, 2, 2, 1
	};

	assert(input);

	if (input->size() < MPEG_AUDIO_HEADER_SIZE) {
		return -1;
	}

    char pBuffer[MPEG_AUDIO_HEADER_SIZE];
    input->input(pBuffer, MPEG_AUDIO_HEADER_SIZE, 0, 0);

	// clear MPEG header
	memset(&m_header, 0, sizeof(m_header));
	// get MPEG header
	m_header.syncWord               = (pBuffer[0] & 0xFF) << 4 | (pBuffer[1] & 0xE0) >> 4;
	m_header.version                = (pBuffer[1] & 0x18) >> 3;
	m_header.layer                  = (pBuffer[1] & 0x06) >> 1;
	m_header.protectionBit          = (pBuffer[1] & 0x01) >> 0;
	m_header.bitRateIndex           = (pBuffer[2] & 0xF0) >> 4;
	m_header.samplingFrequencyIndex = (pBuffer[2] & 0x06) >> 2;
	m_header.paddingBit             = (pBuffer[2] & 0x02) >> 1;
	m_header.privateBit             = (pBuffer[2] & 0x01) >> 0;
	m_header.chMode                 = (pBuffer[3] & 0xC0) >> 6;
	m_header.modeExtension          = (pBuffer[3] & 0x30) >> 4;
	m_header.copyrightBit           = (pBuffer[3] & 0x08) >> 3;
	m_header.originalBit            = (pBuffer[3] & 0x04) >> 2;
	m_header.emphasis               = (pBuffer[3] & 0x03) >> 0;

	if (m_header.syncWord != 0xFFE) {
		return -1;
	}

	m_header.bitRate = bitRate[m_header.version][m_header.bitRateIndex];
	m_header.samplingFrequency = samplingFrequency[m_header.version][m_header.samplingFrequencyIndex];
	m_header.numChannels = numChannels[m_header.chMode];

#ifdef DISPLAY_HEADER
	printf("= MPEG Audio header ===============\n");
	printf("syncWord                 : 0x%X (0xFFE fixed)\n", m_header.syncWord);
	printf("version                  : 0x%X (MPEG %s)\n", m_header.version, version[m_header.version]);
	printf("layer                    : 0x%X (Layer %d)\n", m_header.layer, layer[m_header.layer]);
	printf("protectionBit            : 0x%X\n", m_header.protectionBit);
	printf("bitRateIndex             : 0x%X (%u kbps)\n", m_header.bitRateIndex, m_header.bitRate);
	printf("samplingFrequencyIndex   : 0x%X (%u Hz)\n", m_header.samplingFrequencyIndex, m_header.samplingFrequency);
	printf("paddingBit               : 0x%X\n", m_header.paddingBit);
	printf("privateBit               : 0x%X\n", m_header.privateBit);
	printf("chMode                   : 0x%X (%u channels)\n", m_header.chMode, m_header.numChannels);
	printf("modeExtension            : 0x%X\n", m_header.modeExtension);
	printf("copyrightBit             : 0x%X\n", m_header.copyrightBit);
	printf("originalBit              : 0x%X\n", m_header.originalBit);
	printf("emphasis                 : 0x%X\n", m_header.emphasis);
	printf("===================================\n");
#endif /* DISPLAY_HEADER */

	// header size
	m_headerSize = MPEG_AUDIO_HEADER_SIZE;

	return 0;
}
