/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2013 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */
/*
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <audioout.h>
#include <user_service.h>
#include "at9/audio_decoder_at9.h"
#include "mp3/audio_decoder_mp3.h"
#include "m4aac/audio_decoder_m4aac.h"
#include "audio.h"
#include "../Game.h"

using namespace Media;

#define EVF_BIT_AUDIO_OUT_INIT_END (0x00000001U)
#define EVF_BIT_AUDIO_DEC_INIT_END (0x00010000U)
#define EVF_BIT_AUDIO_INIT_END     (EVF_BIT_AUDIO_OUT_INIT_END | EVF_BIT_AUDIO_DEC_INIT_END)

#define BST_DIR "/app0/audio_video/sound/wave/"
#define PCM_DIR "/hostapp/"

static Audio *s_audio = 0;

static const AudioTrack s_audioTrack[] = {
	// audio output
	{ SCE_AUDIODEC_TYPE_AT9,   BST_DIR "bgm.at9", 0 },
	{ SCE_AUDIODEC_TYPE_MP3,   BST_DIR "bgm.mp3", 0 },
	{ SCE_AUDIODEC_TYPE_M4AAC, BST_DIR "bgm.aac", 0 },
	// file output
//	{ SCE_AUDIODEC_TYPE_AT9,   BST_DIR "bgm.at9", PCM_DIR "bgm_at9.pcm" },
//	{ SCE_AUDIODEC_TYPE_MP3,   BST_DIR "bgm.mp3", PCM_DIR "bgm_mp3.pcm" },
//	{ SCE_AUDIODEC_TYPE_M4AAC, BST_DIR "bgm.aac", PCM_DIR "bgm_aac.pcm" },
};

int Audio::mainDec(const AudioTrack &audioTrack)
{
	int ret = 0;

	// allocate buffer of input file size on memory
	const uint32_t inputStreamSize = File(audioTrack.inputFileName, "rb").size();

	// create an input stream
	m_input = new FileInputStream(inputStreamSize);
	if (!m_input) {
		printf("error: FileInputStream::FileInputStream() failed\n");
		ret = -1;
		goto term;
	}
	printf("audio decoding: <%s>\n", audioTrack.inputFileName);
	// open an input file
	ret = m_input->open(audioTrack.inputFileName, "rb");
	if (ret < 0) {
		printf("error: FileInputStream::open() failed: 0x%08X\n", ret);
		goto term;
	}
	// read whole of an input file
	ret = m_input->input();
	if (ret < 0) {
		printf("error: FileInputStream::input() failed: 0x%08X\n", ret);
		goto term;
	}
	// set stream end
	m_input->end();

	// create an audio decoder
	switch (audioTrack.codecType) {
	case SCE_AUDIODEC_TYPE_AT9:
		m_decoder = new AudioDecoderAt9(m_input);
		break;
	case SCE_AUDIODEC_TYPE_MP3:
		m_decoder = new AudioDecoderMp3(m_input);
		break;
	case SCE_AUDIODEC_TYPE_M4AAC:
		m_decoder = new AudioDecoderM4aac(m_input);
		break;
	default:
		printf("error: unknown codec type: 0x%08X\n", audioTrack.codecType);
		break;
	}
	if (!m_decoder) {
		printf("error: AudioDecoder::AudioDecoder() failed\n");
		ret = -1;
		goto term;
	}

	// notify the parent thread of the end of initialization
	notify(EVF_BIT_AUDIO_DEC_INIT_END);
	// wait for the end of audio thread initialization
	sync();

	// main loop
	while (Game::IsActive()) {
		// check if there is any bitstream
		if (m_input->isEmpty()) {
			break;
		}
		// decode audio data
		ret = m_decoder->decode(m_input, m_output);
		if (ret < 0) {
			printf("error: AudioDecoder::decode() failed: 0x%08X\n", ret);
			goto term;
		}
	}
	// set stream end
	m_output->end();

term:
	// clear flag
	clear();
	// wait for the end of audio thread termination
	sync();
	// delete an audio decoder
	if (m_decoder) {
		delete m_decoder;
		m_decoder = 0;
	}
	// delete an input stream
	if (m_input) {
		delete m_input;
		m_input = 0;
	}
	return ret;
}

static void *threadAudioDecBgm(void *arg)
{
	int ret = 0;

	Audio *audio = reinterpret_cast<Audio *>(arg);
	assert(audio);

	for (uint32_t i = 0; i < sizeof(s_audioTrack) / sizeof(s_audioTrack[0]); i++) {
		ret = audio->mainDec(s_audioTrack[i]);
		if (ret < 0) {
			printf("error: Audio::mainDec() failed: 0x%08X\n", ret);
			break;
		}
		if (!Game::IsActive()) {
			break;
		}
	}
	//g_application.quit();

	return arg;
}

int Audio::mainOut(const AudioTrack &audioTrack)
{
	int ret = 0;

	const uint32_t outputStreamSize = 128 * 1024;
	const uint32_t outputStreamGrain = 256; // sample

	// wait for the end of audio decoding thread initialization
	wait(EVF_BIT_AUDIO_DEC_INIT_END);

	// create an output stream
	if (audioTrack.outputFileName) {
		m_output = new FileOutputStream(outputStreamSize);
	} else {
		m_output = new AudioOutputStream(outputStreamSize, sizeof(int16_t) * AUDIO_STEREO * outputStreamGrain);
	}
	if (!m_output) {
		printf("error: OutputStream::OutputStream() failed\n");
		ret = -1;
		goto term;
	}
	// open an output stream
	if (audioTrack.outputFileName) {
		printf("file output: <%s>\n", audioTrack.outputFileName);
		// open an output file
		ret = m_output->open(audioTrack.outputFileName, "wb");
	} else {
		printf("audio output: %u sample, %u Hz, %u channel(s)\n", outputStreamGrain, m_decoder->sampleRate(), m_decoder->numChannels());
		// open an audio output
		ret = m_output->open(outputStreamGrain, m_decoder->sampleRate(), AudioOutputSystem::param(m_decoder->numChannels()));
	}
	if (ret < 0) {
		printf("error: OutputStream::open() failed: 0x%08X\n", ret);
		goto term;
	}

	// notify the parent thread of the end of initialization
	notify(EVF_BIT_AUDIO_OUT_INIT_END);
	// wait for the end of audio thread initialization
	sync();

	// main loop
	while (Game::IsActive()) {
		// check if there is any bitstream and output
		if (m_input->isEmpty() && m_output->isEmpty()) {
			break;
		}
		// output pcm into stream
		ret = m_output->output();
		if (ret < 0) {
			printf("error: OutputStream::output() failed: 0x%08X\n", ret);
			goto term;
		}
	}

term:
	// wait for the end of audio thread termination
	sync();
	// delete an output stream
	if (m_output) {
		delete m_output;
		m_output = 0;
	}
	return ret;
}

static void *threadAudioOutBgm(void *arg)
{
	int ret = 0;

	Audio *audio = reinterpret_cast<Audio *>(arg);
	assert(audio);

	for (uint32_t i = 0; i < sizeof(s_audioTrack) / sizeof(s_audioTrack[0]); i++) {
		ret = audio->mainOut(s_audioTrack[i]);
		if (ret < 0) {
			printf("error: Audio::mainOut() failed: 0x%08X\n", ret);
			break;
		}
		if (!Game::IsActive()) {
			break;
		}
	}

	return arg;
}

Audio::Audio(void) :
	m_evf(0),
	m_barrier(0),
	m_thrDec(0),
	m_thrOut(0),
	m_input(0),
	m_output(0),
	m_decoder(0)
{
	int ret = 0;

	ScePthreadAttr attr = 0;
	SceKernelSchedParam schedparam;
	const uint32_t stacksize = 512 * 1024;

	// create an event flag
	ret = sceKernelCreateEventFlag(&m_evf, "AudioBgm", SCE_KERNEL_EVF_ATTR_MULTI, 0, 0);
	if (ret < 0) {
		printf("error: sceKernelCreateEventFlag() failed: 0x%08X\n", ret);
		goto term;
	}
	// create a barrier
	ret = scePthreadBarrierInit(&m_barrier, 0, 2, "AudioBgm");
	if (ret < 0) {
		printf("error: scePthreadBarrierInit() failed: 0x%08X\n", ret);
		goto term;
	}

	// create a thread attribute object
	scePthreadAttrInit(&attr);
	// set stack size
	scePthreadAttrSetstacksize(&attr, stacksize);
	// set schedule priority
	scePthreadAttrSetinheritsched(&attr, SCE_PTHREAD_EXPLICIT_SCHED);
	schedparam.sched_priority = SCE_KERNEL_PRIO_FIFO_HIGHEST;
	scePthreadAttrSetschedparam(&attr, &schedparam);
	// create a thread for audio decode
	ret = scePthreadCreate(&m_thrDec, &attr, threadAudioDecBgm, this, "AudioDecBgm");
	if (ret < 0) {
		printf("error: scePthreadCreate() failed: 0x%08X\n", ret);
		goto term;
	}
	// create a thread for audio output
	ret = scePthreadCreate(&m_thrOut, &attr, threadAudioOutBgm, this, "AudioOutBgm");
	if (ret < 0) {
		printf("error: scePthreadCreate() failed: 0x%08X\n", ret);
		goto term;
	}

term:
	if (attr) {
		// delete a thread attribute object
		scePthreadAttrDestroy(&attr);
		attr = 0;
	}
}

Audio::~Audio(void)
{
	// join and detach a thread for audio output
	if (m_thrOut) {
		scePthreadJoin(m_thrOut, 0);
		m_thrOut = 0;
	}
	// join and detach a thread for audio decode
	if (m_thrDec) {
		scePthreadJoin(m_thrDec, 0);
		m_thrDec = 0;
	}
	// delete a barrier
	if (m_barrier) {
		scePthreadBarrierDestroy(&m_barrier);
		m_barrier = 0;
	}
	// delete an event flag
	if (m_evf) {
		sceKernelDeleteEventFlag(m_evf);
		m_evf = 0;
	}
}

int Audio::wait(uint64_t pattern)
{
	return sceKernelWaitEventFlag(m_evf, pattern, SCE_KERNEL_EVF_WAITMODE_AND, 0, 0);
}

int Audio::notify(uint64_t pattern)
{
	return sceKernelSetEventFlag(m_evf, pattern);
}

int Audio::clear(uint64_t pattern)
{
	return sceKernelClearEventFlag(m_evf, pattern);
}

int Audio::sync(void)
{
	return scePthreadBarrierWait(&m_barrier);
}

int Media::initializeAudio(void)
{
	int ret = 0;
	s_audio = new Audio();
	assert(s_audio);
	// wait for the end of thread initialization
	s_audio->wait(EVF_BIT_AUDIO_INIT_END);
	return ret;
}

int Media::finalizeAudio(void)
{
	int ret = 0;
	if (s_audio) {
		delete s_audio;
		s_audio = 0;
	}
	return ret;
}
*/
