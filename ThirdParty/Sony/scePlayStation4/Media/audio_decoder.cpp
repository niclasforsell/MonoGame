/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2013 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#include <stdio.h>
#include <string.h>
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
	m_instance(AUDIODEC_HANDLE_INVALID),
	m_sampleRate(0),
	m_numChannels(0),
	m_frameSize(0),
	m_supFrameCnt(1),
	m_maxBstSize(0),
	m_maxPcmSize(0),
	m_totalSamples(0),
	m_samplesPerBlock(0),
	m_headerSize(0),
	m_numTotalDecode(0),
	m_samplesLastFrame(0),
	m_numNeedlessDecodeFrame(0),
	m_numCancellFrame(0),
	m_numCancelSamples(0),
	m_numRemainData(0),
	m_numFrames(0),
	m_codecType(codecType)
{
	printf("  init: [%5u]\n", m_numFrames);
	memset(m_streamBuf, 0, SCE_AUDIODEC_AT9_MAX_FRAME_SIZE);
	memset(&m_trickInfo, 0, sizeof(m_trickInfo));
	// count of execution loop
	m_trickInfo.m_cntLoop = 1;
}

AudioDecoder::~AudioDecoder(void)
{
	int ret = 0;
	int tmp;

	if (m_instance != AUDIODEC_HANDLE_INVALID) {
		// delete an instance
		tmp = sceAudiodecDeleteDecoder(m_instance);
		if (tmp < 0) {
			printf("error: sceAudiodecDeleteDecoder() failed: 0x%08X\n", tmp);
			ret = (ret < 0) ? ret : tmp;
		}
		m_instance = AUDIODEC_HANDLE_INVALID;
	}

	printf("  term: [%5u]\n", m_numFrames);
}

void AudioDecoder::restart(InputStream *input)
{
	input->input(NULL, 0, m_headerSize, SEEK_SET);
	m_numFrames = 0;
}

int AudioDecoder::decodeNormal(InputStream *input, OutputStream *output, OutputStream *outputFile)
{
	int ret = 0;

	assert(input);
	assert(output);

	void *pPcmFile = NULL;
	uint32_t usedSize = 0;
	uint32_t updateSize = 0;
	int readSize = 0;

	if (outputFile) {
		pPcmFile = outputFile->lock(m_maxPcmSize);
	}
	// get decoding data
	if (m_numRemainData == 0) {
		readSize = input->input((char *)m_streamBuf, m_frameSize, 0, SEEK_CUR);
		if (readSize < 0) {
			ret = readSize;
			goto term;
		}
		m_numRemainData = readSize;
	}
	usedSize = m_frameSize - m_numRemainData;
	// set bitstream and pcm buffer
	m_bst.pAuAddr = m_streamBuf + usedSize;
	m_bst.uiAuSize = m_numRemainData;
	m_pcm.pPcmAddr = output->lock(m_maxPcmSize);
	m_pcm.uiPcmSize = output->writingSize();
	// enqueue a job to decode
	ret = sceAudiodecDecode(m_instance, &m_ctrl);
	if (ret < 0) {
		printf("error: sceAudiodecDecode() failed: 0x%08X\n", ret);
		goto term;
	}
	m_numRemainData -= m_bst.uiAuSize;
	if (((++m_numFrames) % 256) == 0) {
		printf("decode: [%5u]\n", m_numFrames);
	}
	if (m_numFrames != 1) {
		// update offset
		updateSize = m_pcm.uiPcmSize;
	} else {
		// pass the first result
		updateSize = 0;
	}
	// finish decode
	if (m_numTotalDecode <= m_numFrames) {
		input->setIsEmpty(true);
		if (m_samplesLastFrame != 0) {
			updateSize = m_samplesLastFrame * sizeof(int16_t) * m_numChannels;
		}
	}
	// update
	output->unlock(updateSize);
	if (outputFile) {
		// copy output data
		memcpy(pPcmFile, m_pcm.pPcmAddr, updateSize);
		// update
		outputFile->unlock(updateSize);
	}

term:
	return ret;
}

int AudioDecoder::decodeSeek(InputStream *input, OutputStream *output, OutputStream *outputFile)
{
	int ret = 0;

	assert(input);
	assert(output);

	void *pPcmFile = NULL;
	uint32_t usedSize = 0;
	uint32_t updateSize = 0;
	int readSize = 0;

	if (outputFile) {
		pPcmFile = outputFile->lock(m_maxPcmSize);
	}
	// seek start point(first decode)
	if (m_numFrames == 0) {
		readSize = input->input(NULL, 0, (m_numNeedlessDecodeFrame * m_frameSize), SEEK_CUR);
		if (readSize < 0) {
			ret = readSize;
			goto term;
		}
	}
	// get decoding data
	if (m_numRemainData == 0) {
		readSize = input->input((char *)m_streamBuf, m_frameSize, 0, SEEK_CUR);
		if (readSize < 0) {
			ret = readSize;
			goto term;
		}
		m_numRemainData = readSize;
	}
	usedSize = m_frameSize - m_numRemainData;
	// set bitstream and pcm buffer
	m_bst.pAuAddr = m_streamBuf + usedSize;
	m_bst.uiAuSize = m_numRemainData;
	m_pcm.pPcmAddr = output->lock(m_maxPcmSize);
	m_pcm.uiPcmSize = output->writingSize();
	// enqueue a job to decode
	ret = sceAudiodecDecode(m_instance, &m_ctrl);
	if (ret < 0) {
		printf("error: sceAudiodecDecode() failed: 0x%08X\n", ret);
		goto term;
	}
	m_numRemainData -= m_bst.uiAuSize;
	if (((++m_numFrames) % 256) == 0) {
		printf("decode: [%5u]\n", m_numFrames);
	}
	if (m_numFrames != 1) {
		if (m_numCancellFrame != 0) {
			// pass till needless decode count
			updateSize = 0;
			m_numCancellFrame --;
		} else {
			if (m_pcm.uiPcmSize >= (m_numCancelSamples * sizeof(int16_t) * m_numChannels)) {
				// update offset
				updateSize = m_pcm.uiPcmSize - (m_numCancelSamples * sizeof(int16_t) * m_numChannels);
			}
			if (m_numCancelSamples != 0) {
				memcpy((uint8_t *)m_pcm.pPcmAddr, ((uint8_t *)m_pcm.pPcmAddr + (m_numCancelSamples * sizeof(int16_t) * m_numChannels)), updateSize);
			}
			m_numCancelSamples = 0;
		}
	} else {
		// pass the first result
		updateSize = 0;
	}
	// finish decode
	if ((m_numTotalDecode - (m_numNeedlessDecodeFrame * m_supFrameCnt)) <= m_numFrames) {
		input->setIsEmpty(true);
		if (m_samplesLastFrame != 0) {
			updateSize = m_samplesLastFrame * sizeof(int16_t) * m_numChannels;
		}
	}
	// update
	output->unlock(updateSize);
	if (outputFile) {
		// copy output data
		memcpy(pPcmFile, m_pcm.pPcmAddr, updateSize);
		// update
		outputFile->unlock(updateSize);
	}

term:
	return ret;
}

int AudioDecoder::decodeLoop(InputStream *input, OutputStream *output, OutputStream *outputFile)
{
	int ret = 0;

	assert(input);
	assert(output);

	int tmpSampleNum = 0;
	int startSampleNum = m_trickInfo.m_play[0].startSampleNum;
	int endSampleNum = m_trickInfo.m_play[0].endSampleNum;

	if (startSampleNum > endSampleNum) {
		printf("error: Specified position has gone wrong \n");
		printf("     specified start sample point : %d\n", startSampleNum);
		printf("     specified end sample point   : %d\n", endSampleNum);
		ret = -1;
		goto term;
	}
	// no loop info or no loop count
	if ((m_trickInfo.m_numLoop == 0) || (m_trickInfo.m_cntLoop == 0)) {
		ret = decodeNormal(input, output, outputFile);
		goto term;
	}
	if (m_trickInfo.m_partDecEnd == 0) {
		// beginning decode
		tmpSampleNum = endSampleNum;
	} else if (m_trickInfo.m_partDecEnd < m_trickInfo.m_cntLoop) {
		// middle decode
		tmpSampleNum = endSampleNum - startSampleNum;
	} else {
		// last decode
		tmpSampleNum = m_totalSamples;
	}
	// update decode count
	m_numTotalDecode = (tmpSampleNum + (m_samplesPerBlock / m_supFrameCnt - 1)) / (m_samplesPerBlock / m_supFrameCnt) + 1;
	// update number of samples in last frame
	m_samplesLastFrame = tmpSampleNum % (m_samplesPerBlock / m_supFrameCnt);
	if ((m_trickInfo.m_partDecEnd != 0) && (m_numFrames == 0)) {
		// clear context
		ret = sceAudiodecClearContext(m_instance);
		if (ret < 0){
			printf("error: SceAudiodecClearContext() failed: 0x%08X\n", ret);
			goto term;
		}
		// rewind input file
		ret = input->input(NULL, 0, m_headerSize, SEEK_SET);
		if (ret < 0) {
			goto term;
		}
		// update number of needless decoding frames
		m_numNeedlessDecodeFrame = startSampleNum / m_samplesPerBlock;
		if (m_supFrameCnt != 1) {
			// update number of canceling frames
			m_numCancellFrame = (startSampleNum % m_samplesPerBlock) / (m_samplesPerBlock / m_supFrameCnt);
		}
		// update number of canceling samples
		m_numCancelSamples = startSampleNum % (m_samplesPerBlock / m_supFrameCnt);
	}
	if (m_trickInfo.m_partDecEnd == 0) {
		// beginning decode
		ret = decodeNormal(input, output, outputFile);
		if (ret < 0) {
			goto term;
		}
	} else {
		// middle decode & last decode
		ret = decodeSeek(input, output, outputFile);
		if (ret < 0) {
			goto term;
		}
	}
	// finish decode
	if (input->isEmpty()) {
		if (m_trickInfo.m_partDecEnd < m_trickInfo.m_cntLoop) {
			input->setIsEmpty(false);
			printf("  term: [%5u]\n", m_numFrames);
			m_numFrames = 0;
		}
		m_trickInfo.m_partDecEnd ++;
		m_numRemainData = 0;
	}

term:
	return ret;
}

float AudioDecoder::getProgress()
{
	return (float)m_numFrames / (float)m_numTotalDecode;
}
