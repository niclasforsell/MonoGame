/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2013 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <audioout.h>
#include "riff_wave_header_at9_parser.h"
#include "audio_decoder_at9.h"

using namespace Media;

AudioDecoderAt9::AudioDecoderAt9(InputStream *input, AudioDecoderTrickPlayPoint playPoint, int *pRet) : AudioDecoder(SCE_AUDIODEC_TYPE_AT9)
{
	int frameSamples = 0;
	int i = 0;

	RiffWaveHeaderAt9Parser parser;
	const RiffWaveHeaderAt9 &header = parser.header();

	assert(input);

	// parse the header
	*pRet = parser.parse(input);
	if (*pRet < 0) {
		printf("error: RiffWaveHeaderAt9Parser::parse() failed: 0x%08X\n", *pRet);
		return;
	}
	// set initial values
	m_ctrl.pParam = reinterpret_cast<void *>(&m_param.at9);
	m_param.at9.uiSize = sizeof(m_param.at9);
	m_param.at9.iBwPcm = SCE_AUDIODEC_WORD_SZ_16BIT;
	// copy the ATRAC9 config data
	memcpy(m_param.at9.uiConfigData, header.fmtChunk.configData, sizeof(m_param.at9.uiConfigData));
	m_ctrl.pBsiInfo = reinterpret_cast<void *>(&m_info.at9);
	m_info.at9.uiSize = sizeof(m_info.at9);
	m_ctrl.pAuInfo = &m_bst;
	m_bst.uiSize = sizeof(m_bst);
	m_ctrl.pPcmItem = &m_pcm;
	m_pcm.uiSize = sizeof(m_pcm);
	// create an instance
	*pRet = sceAudiodecCreateDecoder(&m_ctrl, SCE_AUDIODEC_TYPE_AT9);
	if (*pRet < 0) {
		printf("error: sceAudiodecCreateDecoder() failed: 0x%08X\n", *pRet);
		return;
	}
	m_instance = *pRet;
	// check start point
	if (header.factChunk.totalSamples <= playPoint.startSampleNum) {
		printf("error: Specified position is over the termination of a file \n");
		printf("     total samples num      : %d\n", header.factChunk.totalSamples);
		printf("     specified sample point : %d\n", playPoint.startSampleNum);
		*pRet = -1;
		return;
	}
	// sample rate
	m_sampleRate = header.fmtChunk.samplesPerSec;
	switch (m_sampleRate) {
	case 48000:
		frameSamples = SCE_AUDIODEC_AT9_MAX_FRAME_SAMPLES;
		break;
	case 24000:
		frameSamples = SCE_AUDIODEC_AT9_MAX_FRAME_SAMPLES / 2;
		break;
	case 12000:
		frameSamples = SCE_AUDIODEC_AT9_MAX_FRAME_SAMPLES / 4;
		break;
	default:
		printf("error: unsupported sampling rate: %d\n", m_sampleRate);
		break;
	}
	// number of channels
	m_numChannels = header.fmtChunk.numChannels;
	// frame size
	m_frameSize = header.fmtChunk.blockAlign;
	// super frame ON(=4)/OFF(=1)
	m_supFrameCnt = header.fmtChunk.samplesPerBlock / frameSamples;
	// maximum bitstream size
	m_maxBstSize = SCE_AUDIODEC_AT9_MAX_FRAME_SIZE;
	// maximum pcm size
	m_maxPcmSize = sizeof(int16_t) * m_numChannels * SCE_AUDIODEC_AT9_MAX_FRAME_SAMPLES;
	// total samples
	m_totalSamples = header.factChunk.totalSamples;
	// samples per block
	m_samplesPerBlock = header.fmtChunk.samplesPerBlock;
	// header size
	m_headerSize = parser.headerSize();
	// number of total decoding
	m_numTotalDecode = (m_totalSamples + (m_samplesPerBlock / m_supFrameCnt - 1)) / (m_samplesPerBlock / m_supFrameCnt) + 1;
	// number of samples in last frame
	m_samplesLastFrame = m_totalSamples % (m_samplesPerBlock / m_supFrameCnt);
	// number of needless decoding frames
	m_numNeedlessDecodeFrame = playPoint.startSampleNum / m_samplesPerBlock;
	if (m_supFrameCnt != 1) {
		// number of canceling frames
		m_numCancellFrame = (playPoint.startSampleNum % m_samplesPerBlock) / (m_samplesPerBlock / m_supFrameCnt);
	}
	// number of canceling samples
	m_numCancelSamples = playPoint.startSampleNum % (m_samplesPerBlock / m_supFrameCnt);
	// number of loop
	m_trickInfo.m_numLoop = header.smplChunk.sampleLoops;
	// loop info
	if (playPoint.startSampleNum != playPoint.endSampleNum) {
		m_trickInfo.m_play[0].startSampleNum = playPoint.startSampleNum;
		m_trickInfo.m_play[0].endSampleNum = playPoint.endSampleNum;
		m_trickInfo.m_numLoop = 1;
	} else {
		for (i = 0; i < m_trickInfo.m_numLoop; i++) {
			if (i >=  STREAM_LOOPINFO_MAX) {
				// cancel
				m_trickInfo.m_numLoop = STREAM_LOOPINFO_MAX;
				break;
			}
			m_trickInfo.m_play[i].startSampleNum = header.smplChunk.sampleLoop[i].start - header.factChunk.delaySamplesInputOverlapEncoder;
			m_trickInfo.m_play[i].endSampleNum = header.smplChunk.sampleLoop[i].end - header.factChunk.delaySamplesInputOverlapEncoder + 1;
		}
	}
}
