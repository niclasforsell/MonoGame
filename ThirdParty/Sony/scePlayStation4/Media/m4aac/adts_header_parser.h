/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2012 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#ifndef _ADTS_HEADER_PARSER_H
#define _ADTS_HEADER_PARSER_H

#include <stdint.h>
#include "../stream.h"

namespace Media {

typedef struct AdtsHeader {
	uint32_t syncWord;
	uint32_t id;
	uint32_t layer;
	uint32_t protectionAbsent;
	uint32_t profile;
	uint32_t samplingFrequencyIndex;
	uint32_t privateBit;
	uint32_t channelConfiguration;
	uint32_t original;
	uint32_t home;
	uint32_t samplingFrequency;
	uint32_t numChannels;
} AdtsHeader;

class AdtsHeaderParser
{
public:
	AdtsHeaderParser(void) : m_headerSize(0) {}
	~AdtsHeaderParser(void) {}
	const AdtsHeader &header() { return m_header; }
	uint32_t headerSize(void) { return m_headerSize; }
	int parse(InputStream *input);
private:
	AdtsHeader m_header;
	uint32_t m_headerSize;
};

} // namespace Media

#endif /* _ADTS_HEADER_PARSER_H */
