/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2012 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "adts_header_parser.h"

using namespace Media;

#define ADTS_HEADER_SIZE (4)

int AdtsHeaderParser::parse(const uint8_t *pBuffer, uint32_t bufferSize)
{
	const uint32_t samplingFrequency[] = {
		96000,
		88200,
		64000,
		48000,
		44100,
		32000,
		24000,
		22050,
		16000,
		12000,
		11025,
		8000,
		0,
		0,
		0,
		0,
	};

	assert(pBuffer);

	if (bufferSize < ADTS_HEADER_SIZE) {
		return -1;
	}

	// clear ADTS header
	memset(&m_header, 0, sizeof(m_header));
	// get ADTS header
	m_header.syncWord                 = (pBuffer[0] & 0xFF) << 4 | (pBuffer[1] & 0xF0) >> 4;
	m_header.id                       = (pBuffer[1] & 0x08) >> 3;
	m_header.layer                    = (pBuffer[1] & 0x06) >> 1;
	m_header.protectionAbsent         = (pBuffer[1] & 0x01) >> 0;
	m_header.profile                  = (pBuffer[2] & 0xC0) >> 6;
	m_header.samplingFrequencyIndex   = (pBuffer[2] & 0x3C) >> 2;
	m_header.privateBit               = (pBuffer[2] & 0x02) >> 1;
	m_header.channelConfiguration     = (pBuffer[2] & 0x01) << 2 | (pBuffer[3] & 0xC0) >> 6;
	m_header.original                 = (pBuffer[3] & 0x20) >> 5;
	m_header.home                     = (pBuffer[3] & 0x10) >> 4;

	if (m_header.syncWord != 0xFFF) {
		return -1;
	}

	m_header.samplingFrequency = samplingFrequency[m_header.samplingFrequencyIndex];
	m_header.numChannels = m_header.channelConfiguration;

#ifdef DISPLAY_HEADER
	printf("= ADTS fixed header ===============\n");
	printf("syncWord                 : 0x%X (0xFFF fixed)\n", m_header.syncWord);
	printf("id                       : 0x%X\n", m_header.id);
	printf("layer                    : 0x%X\n", m_header.layer);
	printf("protectionAbsent         : 0x%X\n", m_header.protectionAbsent);
	printf("profile                  : 0x%X\n", m_header.profile);
	printf("samplingFrequencyIndex   : 0x%X (%u Hz)\n", m_header.samplingFrequencyIndex, m_header.samplingFrequency);
	printf("privateBit               : 0x%X\n", m_header.privateBit);
	printf("channelConfiguration     : 0x%X\n", m_header.channelConfiguration);
	printf("original                 : 0x%X\n", m_header.original);
	printf("home                     : 0x%X\n", m_header.home);
	printf("===================================\n");
#endif /* DISPLAY_HEADER */

	// header size
	m_headerSize = ADTS_HEADER_SIZE;

	return 0;
}
