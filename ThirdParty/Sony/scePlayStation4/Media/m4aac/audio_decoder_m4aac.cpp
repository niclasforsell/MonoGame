/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2012 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <audioout.h>
#include "adts_header_parser.h"
#include "audio_decoder_m4aac.h"

using namespace Media;

#define CONFIG_NUMBER_HEADER_TYPE_ADTS (1)

AudioDecoderM4aac::AudioDecoderM4aac(InputStream *input) : AudioDecoder(SCE_AUDIODEC_TYPE_M4AAC)
{
	int ret = 0;

	AdtsHeaderParser parser;
	const AdtsHeader &header = parser.header();

	assert(input);

	// parse the header
	ret = parser.parse(input->lock(), input->readingSize());
	if (ret < 0) {
		printf("error: AdtsHeaderParser::parse() failed: 0x%08X\n", ret);
		goto term;
	}

	// set the head of file to the read point
	input->unlock(0);

	// set initial values
	m_ctrl.pParam = reinterpret_cast<void *>(&m_param.m4aac);
	m_param.m4aac.uiSize = sizeof(m_param.m4aac);
	m_param.m4aac.iBwPcm = SCE_AUDIODEC_WORD_SZ_16BIT;
	m_param.m4aac.uiConfigNumber = CONFIG_NUMBER_HEADER_TYPE_ADTS;
	m_param.m4aac.uiSamplingFreqIndex = header.samplingFrequencyIndex;
	m_param.m4aac.uiMaxChannels = header.numChannels;
	m_param.m4aac.uiEnableHeaac = 0;
	m_ctrl.pBsiInfo = reinterpret_cast<void *>(&m_info.m4aac);
	m_info.m4aac.uiSize = sizeof(m_info.m4aac);
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
	m_sampleRate = header.samplingFrequency;
	// number of channels
	m_numChannels = header.numChannels;
	// maximum bitstream size
	m_maxBstSize = SCE_AUDIODEC_M4AAC_MAX_FRAME_SIZE;
	// maximum pcm size
	m_maxPcmSize = sizeof(int32_t) * header.numChannels * SCE_AUDIODEC_M4AAC_MAX_FRAME_SAMPLES;

term:
	return;
}
