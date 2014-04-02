/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2012 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#ifndef _AUDIO_DECODER_H
#define _AUDIO_DECODER_H

#include <stdint.h>
#include <audiodec.h>
#include "stream.h"

namespace Media {

#define AUDIODEC_HANDLE_INVALID ((int32_t)-1)

typedef union AudiodecParam {
	SceAudiodecParamAt9 at9;
	SceAudiodecParamMp3 mp3;
	SceAudiodecParamM4aac m4aac;
} AudiodecParam;

typedef union AudiodecInfo {
	SceAudiodecAt9Info at9;
	SceAudiodecMp3Info mp3;
	SceAudiodecM4aacInfo m4aac;
} AudiodecInfo;

class AudioCodecSystem
{
public:
	AudioCodecSystem(void);
	~AudioCodecSystem(void);
	static uint32_t maxBstSize(uint32_t codecType);
private:
	static const uint32_t s_supportedCodecType[];
};

class AudioDecoder
{
public:
	AudioDecoder(uint32_t codecType);
	virtual ~AudioDecoder(void);
	int decode(InputStream *input, OutputStream *output);
	uint32_t codecType(void) { return m_codecType; }
	uint32_t sampleRate(void) { return m_sampleRate; }
	uint32_t numChannels(void) { return m_numChannels; }
protected:
	static AudioCodecSystem s_system;
	int32_t m_handle;
	SceAudiodecCtrl m_ctrl;
	SceAudiodecAuInfo m_bst;
	SceAudiodecPcmItem m_pcm;
	AudiodecParam m_param;
	AudiodecInfo m_info;
	uint32_t m_sampleRate;
	uint32_t m_numChannels;
	uint32_t m_maxBstSize;
	uint32_t m_maxPcmSize;
	uint32_t m_numFrames;
private:
	const uint32_t m_codecType;
};

} // namespace Media
#endif /* _AUDIO_DECODER_H */
