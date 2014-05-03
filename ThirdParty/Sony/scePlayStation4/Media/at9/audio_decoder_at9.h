/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2013 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#ifndef _AUDIO_DECODER_AT9_H
#define _AUDIO_DECODER_AT9_H

#include <stdint.h>
#include "../stream.h"
#include "../audio_decoder.h"

namespace Media {

class AudioDecoderAt9 : public AudioDecoder
{
public:
	AudioDecoderAt9(InputStream *input);
	~AudioDecoderAt9(void) {}
};

} // namespace Media

#endif /* _AUDIO_DECODER_AT9_H */
