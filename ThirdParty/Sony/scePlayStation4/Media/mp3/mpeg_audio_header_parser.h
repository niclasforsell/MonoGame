/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2012 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#ifndef _MPEG_AUDIO_HEADER_PARSER_H
#define _MPEG_AUDIO_HEADER_PARSER_H

#include <stdint.h>
#include "../stream.h"

namespace Media {

#define MPEG_VERSION_2_5      (0) /* MPEG2.5 */
#define MPEG_VERSION_RESERVED (1) /* reserved */
#define MPEG_VERSION_2        (2) /* MPEG2 */
#define MPEG_VERSION_1        (3) /* MPEG1 */

typedef struct MpegAudioHeader {
	uint32_t syncWord;
	uint32_t version;
	uint32_t layer;
	uint32_t protectionBit;
	uint32_t bitRateIndex;
	uint32_t samplingFrequencyIndex;
	uint32_t paddingBit;
	uint32_t privateBit;
	uint32_t chMode;
	uint32_t modeExtension;
	uint32_t copyrightBit;
	uint32_t originalBit;
	uint32_t emphasis;
	uint32_t bitRate;
	uint32_t samplingFrequency;
	uint32_t numChannels;
} MpegAudioHeader;

class MpegAudioHeaderParser
{
public:
	MpegAudioHeaderParser(void) : m_headerSize(0) {}
	~MpegAudioHeaderParser(void) {}
	const MpegAudioHeader &header() { return m_header; }
	uint32_t headerSize(void) { return m_headerSize; }
	int parse(InputStream *input);
private:
	MpegAudioHeader m_header;
	uint32_t m_headerSize;
};

} // namespace Media

#endif /* _MPEG_AUDIO_HEADER_PARSER_H */
