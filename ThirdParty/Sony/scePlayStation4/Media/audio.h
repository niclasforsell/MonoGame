/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2012 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */
/*
#ifndef _AUDIO_H
#define _AUDIO_H

#include <stdint.h>
#include <kernel.h>
#include "stream.h"
#include "audio_decoder.h"

namespace Media {

typedef struct AudioTrack {
	uint32_t codecType;
	const char *inputFileName;
	const char *outputFileName;
} AudioTrack;

class Audio
{
public:
	Audio(void);
	~Audio(void);
	int wait(uint64_t pattern);
	int notify(uint64_t pattern);
	int clear(uint64_t pattern = 0);
	int sync(void);
	int mainDec(const AudioTrack &audioTrack);
	int mainOut(const AudioTrack &audioTrack);
private:
	SceKernelEventFlag m_evf;
	ScePthreadBarrier m_barrier;
	ScePthread m_thrDec;
	ScePthread m_thrOut;
	InputStream *m_input;
	OutputStream *m_output;
	AudioDecoder *m_decoder;
};

int initializeAudio(void);
int finalizeAudio(void);

} // namespace Media

#endif /* _AUDIO_H */
