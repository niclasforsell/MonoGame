/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2012 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#ifndef _AUDIO_DECODER_M4AAC_H
#define _AUDIO_DECODER_M4AAC_H

#include <stdint.h>
#include "../stream.h"
#include "../audio_decoder.h"

namespace Media {

class AudioDecoderM4aac : public AudioDecoder
{
public:
	AudioDecoderM4aac(InputStream *input);
	~AudioDecoderM4aac(void) {}
};

} // namespace Media

#endif /* _AUDIO_DECODER_M4AAC_H */
