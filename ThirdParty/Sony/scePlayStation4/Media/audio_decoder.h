/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2013 Sony Computer Entertainment Inc.
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

#define ATRAC9_DECPLAY_NORMAL (0)
#define ATRAC9_DECPLAY_SEEK (1)
#define ATRAC9_DECPLAY_LOOP (2)

typedef struct AudioDecoderTrickPlayPoint {
	uint32_t startSampleNum;
	uint32_t endSampleNum;
} AudioDecoderTrickPlayPoint;

typedef struct AudioDecoderTrickPlayMng {
	uint32_t m_numLoop;
	uint32_t m_cntLoop;
	uint32_t m_partDecEnd;
	AudioDecoderTrickPlayPoint m_play[STREAM_LOOPINFO_MAX];
} AudioDecoderTrickPlayMng;

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
	void restart(InputStream *input);
	int decodeNormal(InputStream *input, OutputStream *output, OutputStream *outputFile);
	int decodeSeek(InputStream *input, OutputStream *output, OutputStream *outputFile);
	int decodeLoop(InputStream *input, OutputStream *output, OutputStream *outputFile);
	float getProgress(void);
	uint32_t codecType(void) { return m_codecType; }
	uint32_t sampleRate(void) { return m_sampleRate; }
	uint32_t numChannels(void) { return m_numChannels; }
protected:
	static AudioCodecSystem s_system;
	int32_t m_instance;
	SceAudiodecCtrl m_ctrl;
	SceAudiodecAuInfo m_bst;
	SceAudiodecPcmItem m_pcm;
	AudiodecParam m_param;
	AudiodecInfo m_info;
	uint32_t m_sampleRate;
	uint32_t m_numChannels;
	uint32_t m_frameSize;
	uint32_t m_supFrameCnt;
	uint32_t m_maxBstSize;
	uint32_t m_maxPcmSize;
	uint32_t m_totalSamples;
	uint32_t m_samplesPerBlock;
	uint32_t m_headerSize;
	uint32_t m_numTotalDecode;
	uint32_t m_samplesLastFrame;
	uint32_t m_numNeedlessDecodeFrame;
	uint32_t m_numCancellFrame;
	uint32_t m_numCancelSamples;
	uint32_t m_numRemainData;
	uint32_t m_numFrames;
	uint8_t m_streamBuf[SCE_AUDIODEC_AT9_MAX_FRAME_SIZE];
	AudioDecoderTrickPlayMng m_trickInfo;
private:
	const uint32_t m_codecType;
};

} // namespace Media
#endif /* _AUDIO_DECODER_H */
