/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2013 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#include <stdio.h>
#include <assert.h>
#include <user_service.h>
#include <audioout.h>
#include "audio_output.h"

using namespace Media;

AudioOutputSystem::AudioOutputSystem(void)
{
	int ret = 0;

	// initialize audio output
	ret = sceAudioOutInit();
	if (ret < 0) {
		printf("error: sceAudioOutInit() failed: 0x%08X\n", ret);
		goto term;
	}

term:
	return;
}

int32_t AudioOutputSystem::param(uint32_t numChannels)
{
	int32_t param = 0;

	switch (numChannels) {
	case AUDIO_MONAURAL:
		param = SCE_AUDIO_OUT_PARAM_FORMAT_S16_MONO;
		break;
	case AUDIO_STEREO:
		param = SCE_AUDIO_OUT_PARAM_FORMAT_S16_STEREO;
		break;
	case AUDIO_SURROUND:
		param = SCE_AUDIO_OUT_PARAM_FORMAT_S16_8CH;
		break;
	default:
		printf("error: unsupported number of channels: %d\n", param);
		break;
	}

	return param;
}

uint32_t AudioOutputSystem::numChannels(int32_t param)
{
	uint32_t numChannels = 0;

	switch (param) {
	case SCE_AUDIO_OUT_PARAM_FORMAT_S16_MONO:
		numChannels = AUDIO_MONAURAL;
		break;
	case SCE_AUDIO_OUT_PARAM_FORMAT_S16_STEREO:
		numChannels = AUDIO_STEREO;
		break;
	case SCE_AUDIO_OUT_PARAM_FORMAT_S16_8CH:
		numChannels = AUDIO_SURROUND;
		break;
	default:
		printf("error: unsupported number of channels: %u\n", numChannels);
		break;
	}

	return numChannels;
}

AudioOutputSystem AudioOutput::s_system;

AudioOutput::AudioOutput(void)
{
	// set initial values
	m_handle = 0;
	m_grain = 0;
	m_sampleRate = 0;
	m_param = 0;
	m_numChannels = 0;

	m_volume[0] = SCE_AUDIO_VOLUME_0dB / 4;
	m_volume[1] = SCE_AUDIO_VOLUME_0dB / 4;
}

AudioOutput::~AudioOutput(void)
{
	close();
}

int AudioOutput::open(int32_t grain, int32_t sampleRate, int32_t param)
{
	int ret = 0;

	// open audio port
	ret = sceAudioOutOpen(SCE_USER_SERVICE_USER_ID_SYSTEM, SCE_AUDIO_OUT_PORT_TYPE_BGM, 0, grain, sampleRate, param);
	if (ret < 0) {
		printf("error: sceAudioOutOpen() failed: 0x%08X\n", ret);
		goto term;
	}
	m_handle = ret;
	m_grain = grain;
	m_sampleRate = sampleRate;
	m_param = param;
	m_numChannels = AudioOutputSystem::numChannels(param);
	ret = 0;

	// set volume
	ret = sceAudioOutSetVolume(m_handle, (SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH), m_volume);
	if (ret < 0) {
		printf("error: sceAudioOutSetVolume() failed: 0x%08X\n", ret);
		goto term;
	}

term:
	return ret;
}

int AudioOutput::close(void)
{
	int ret = 0;
	int tmp;

	if (0 < m_handle) {
		// output remaining audio
		sceAudioOutOutput(m_handle, 0);
		// close audio port
		tmp = sceAudioOutClose(m_handle);
		if (tmp < 0) {
			printf("error: sceAudioOutClose() failed: 0x%08X\n", tmp);
			ret = (ret < 0) ? ret : tmp;
		}
		// set initial values
		m_handle = 0;
		m_grain = 0;
		m_sampleRate = 0;
		m_param = 0;
		m_numChannels = 0;
	}

	return ret;
}

int AudioOutput::output(const void *p)
{
	int ret = 0;

	// output audio data
	ret = sceAudioOutOutput(m_handle, p);
	if (ret < 0) {
		printf("error: sceAudioOutOutput() failed: 0x%08X\n", ret);
		goto term;
	}

term:
	return ret;
}

float AudioOutput::getVolume()
{
	return (float)(m_volume[0] / SCE_AUDIO_VOLUME_0DB);
}

void AudioOutput::setVolume(float value)
{
	if (value > 1.0f)
		value = 1.0f;

	if (value < 0.0f)
		value = 0.0f;

	m_volume[0] = m_volume[1] = (int32_t)(value * SCE_AUDIO_VOLUME_0DB);

	auto ret = sceAudioOutSetVolume(m_handle, (SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH), m_volume);
	if (ret < 0) {
		printf("error: sceAudioOutSetVolume() failed: 0x%08X\n", ret);
	}
}
