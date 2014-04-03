/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2012 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <audioout.h>
#include "mpeg_audio_header_parser.h"
#include "audio_decoder_mp3.h"

using namespace Media;

AudioDecoderMp3::AudioDecoderMp3(InputStream *input) : AudioDecoder(SCE_AUDIODEC_TYPE_MP3)
{
	int ret = 0;

	MpegAudioHeaderParser parser;
	const MpegAudioHeader &header = parser.header();

	assert(input);

	// parse the header
	ret = parser.parse(input);
	if (ret < 0) {
		printf("error: MpegAudioHeaderParser::parse() failed: 0x%08X\n", ret);
		goto term;
	}

	// set initial values
	m_ctrl.pParam = reinterpret_cast<void *>(&m_param.mp3);
	m_param.mp3.uiSize = sizeof(m_param.mp3);
	m_param.mp3.iBwPcm = SCE_AUDIODEC_WORD_SZ_16BIT;
	m_ctrl.pBsiInfo = reinterpret_cast<void *>(&m_info.mp3);
	m_info.mp3.uiSize = sizeof(m_info.mp3);
	m_ctrl.pAuInfo = &m_bst;
	m_bst.uiSize = sizeof(m_bst);
	m_ctrl.pPcmItem = &m_pcm;
	m_pcm.uiSize = sizeof(m_pcm);

	// create an instance
	ret = sceAudiodecCreateDecoder(&m_ctrl, codecType());
	if (ret < 0) {
		printf("error: sceAudiodecCreateDecoder() failed: 0x%08X\n", ret);
		goto term;
	}
	m_instance = ret;

	// sample rate
	m_sampleRate = header.samplingFrequency;
	// number of channels
	m_numChannels = header.numChannels;
	// maximum bitstream size
	m_maxBstSize = MP3_MAX_FRAME_SIZE;
	// maximum pcm size
	m_maxPcmSize = sizeof(int16_t) * m_numChannels * MP3_MAX_FRAME_SAMPLES;

term:
	return;
}
