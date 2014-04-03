/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2012 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#ifndef _AUDIO_DECODER_MP3_H
#define _AUDIO_DECODER_MP3_H

#include <stdint.h>
#include "../stream.h"
#include "../audio_decoder.h"

namespace Media {

#define MP3_MAX_FRAME_SIZE    (1441)
#define MP3_MAX_FRAME_SAMPLES (1152)

class AudioDecoderMp3 : public AudioDecoder
{
public:
	AudioDecoderMp3(InputStream *input);
	~AudioDecoderMp3(void) {}
};

} // namespace Media

#endif /* _AUDIO_DECODER_MP3_H */
