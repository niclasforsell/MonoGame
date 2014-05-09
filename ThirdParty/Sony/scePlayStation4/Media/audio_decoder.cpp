/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2013 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#include <stdio.h>
#include <assert.h>
#include <libsysmodule.h>
#include <audioout.h>
#include "audio_decoder.h"
#include "mp3/audio_decoder_mp3.h"
#include "m4aac/audio_decoder_m4aac.h"

using namespace Media;

const uint32_t AudioCodecSystem::s_supportedCodecType[] = {
	SCE_AUDIODEC_TYPE_AT9,
	SCE_AUDIODEC_TYPE_MP3,
	SCE_AUDIODEC_TYPE_M4AAC,
};

AudioCodecSystem::AudioCodecSystem(void)
{
	int ret = 0;

	// load the libaudiodec module
	ret = sceSysmoduleLoadModule(SCE_SYSMODULE_AUDIODEC);
	if (ret < 0) {
		printf("error: sceSysmoduleLoadModule() failed: 0x%08X\n", ret);
		return;
	}

	// register all the codecs on libaudiodec
	for (uint32_t i = 0; i < sizeof(s_supportedCodecType) / sizeof(s_supportedCodecType[0]); i++) {
		ret = sceAudiodecInitLibrary(s_supportedCodecType[i]);
		if (ret < 0) {
			printf("error: sceAudiodecInitLibrary() failed: 0x%08X\n", ret);
			return;
		}
	}
}

AudioCodecSystem::~AudioCodecSystem(void)
{
	int ret = 0;
	int tmp;

	// unregister all the codecs from libaudiodec
	for (uint32_t i = 0; i < sizeof(s_supportedCodecType) / sizeof(s_supportedCodecType[0]); i++) {
		tmp = sceAudiodecTermLibrary(s_supportedCodecType[i]);
		if (tmp < 0) {
			printf("error: sceAudiodecTermLibrary() failed: 0x%08X\n", tmp);
			ret = (ret < 0) ? ret : tmp;
		}
	}
	// unload the libaudiodec module
	if (sceSysmoduleIsLoaded(SCE_SYSMODULE_AUDIODEC)) {
		tmp = sceSysmoduleUnloadModule(SCE_SYSMODULE_AUDIODEC);
		if (tmp < 0) {
			printf("error: sceSysmoduleUnloadModule() failed: 0x%08X\n", tmp);
			ret = (ret < 0) ? ret : tmp;
		}
	}
}

uint32_t AudioCodecSystem::maxBstSize(uint32_t codecType)
{
	uint32_t maxBstSize = 0;

	switch (codecType) {
	case SCE_AUDIODEC_TYPE_AT9:
		maxBstSize = SCE_AUDIODEC_AT9_MAX_FRAME_SIZE;
		break;
	case SCE_AUDIODEC_TYPE_MP3:
		maxBstSize = MP3_MAX_FRAME_SIZE;
		break;
	case SCE_AUDIODEC_TYPE_M4AAC:
		maxBstSize = SCE_AUDIODEC_M4AAC_MAX_FRAME_SIZE;
		break;
	default:
		printf("error: unknown codec type: 0x%08X\n", codecType);
		break;
	}

	return maxBstSize;
}

AudioCodecSystem AudioDecoder::s_system;

AudioDecoder::AudioDecoder(uint32_t codecType) :
	m_handle(AUDIODEC_HANDLE_INVALID),
	m_sampleRate(0),
	m_numChannels(0),
	m_maxBstSize(0),
	m_maxPcmSize(0),
	m_numFrames(0),
	m_codecType(codecType),
	m_elapsedSeconds(0.0f)
{
	printf("  init: [%5u]\n", m_numFrames);
}

AudioDecoder::~AudioDecoder(void)
{
	int ret = 0;
	int tmp;

	if (m_handle != AUDIODEC_HANDLE_INVALID) {
		// delete an instance
		tmp = sceAudiodecDeleteDecoder(m_handle);
		if (tmp < 0) {
			printf("error: sceAudiodecDeleteDecoder() failed: 0x%08X\n", tmp);
			ret = (ret < 0) ? ret : tmp;
		}
		m_handle = AUDIODEC_HANDLE_INVALID;
	}

	printf("  term: [%5u]\n", m_numFrames);
}

int AudioDecoder::decode(InputStream *input, OutputStream *output)
{
	int ret = 0;

	assert(input);
	assert(output);

	// set bitstream and pcm buffer
	m_bst.pAuAddr = const_cast<uint8_t *>(input->lock(m_maxBstSize));
	m_bst.uiAuSize = input->readingSize();
	m_pcm.pPcmAddr = output->lock(m_maxPcmSize);
	m_pcm.uiPcmSize = output->writingSize();

	// decode audio bitstream
	ret = sceAudiodecDecode(m_handle, &m_ctrl);
	if (ret < 0) {
		printf("error: sceAudiodecDecode() failed: 0x%08X\n", ret);
		goto term;
	}

term:
	// update offset
	input->unlock(m_bst.uiAuSize);
	output->unlock(m_pcm.uiPcmSize);

	if (((++m_numFrames) % 256) == 0) {
		printf("decode: [%5u]\n", m_numFrames);
	}

	auto rate = sizeof(int16_t) * m_sampleRate * m_numChannels;
	m_elapsedSeconds += (float)m_pcm.uiPcmSize / (float)rate;

	return ret;
}

float AudioDecoder::getElapsedSeconds()
{
	return m_elapsedSeconds;
}
