/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2012 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <audioout.h>
#include "riff_wave_header_at9_parser.h"
#include "audio_decoder_at9.h"

using namespace Media;

AudioDecoderAt9::AudioDecoderAt9(InputStream *input) : AudioDecoder(SCE_AUDIODEC_TYPE_AT9)
{
	int ret = 0;

	RiffWaveHeaderAt9Parser parser;
	const RiffWaveHeaderAt9 &header = parser.header();

	assert(input);

	// parse the header
	ret = parser.parse(input->lock(), input->readingSize());
	if (ret < 0) {
		printf("error: RiffWaveHeaderAt9Parser::parse() failed: 0x%08X\n", ret);
		goto term;
	}

	// set the end of header to the read point
	input->unlock(parser.headerSize());

	// set initial values
	m_ctrl.pParam = reinterpret_cast<void *>(&m_param.at9);
	m_param.at9.uiSize = sizeof(m_param.at9);
	m_param.at9.iBwPcm = SCE_AUDIODEC_WORD_SZ_16BIT;
	memcpy(m_param.at9.uiConfigData, header.fmtChunk.configData, sizeof(m_param.at9.uiConfigData));
	m_ctrl.pBsiInfo = reinterpret_cast<void *>(&m_info.at9);
	m_info.at9.uiSize = sizeof(m_info.at9);
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
	m_handle = ret;

	// sample rate
	m_sampleRate = header.fmtChunk.samplesPerSec;
	// number of channels
	m_numChannels = header.fmtChunk.numChannels;
	// maximum bitstream size
	m_maxBstSize = SCE_AUDIODEC_AT9_MAX_FRAME_SIZE;
	// maximum pcm size
	m_maxPcmSize = sizeof(int16_t) * m_numChannels * SCE_AUDIODEC_AT9_MAX_FRAME_SAMPLES;

term:
	return;
}
