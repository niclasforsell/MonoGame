#include "musicPlayer.h"

#include <assert.h>
#include <audiodec.h>
#include <sceerror.h>

using namespace Media;

namespace {
	SceAudiodecCtrl audioDecCtrl;
	SceAudiodecParamM4aac paramAac;
	SceAudiodecM4aacInfo aacInfo;
	SceAudiodecAuInfo auInfo;
	SceAudiodecPcmItem pcmItem;

	int32_t playerHandle;
	uint32_t playerSampleRate;
	uint32_t playerNumChannels;
}

void MusicPlayer::Initialize()
{
	auto ret = sceAudiodecInitLibrary(SCE_AUDIODEC_TYPE_M4AAC);
	assert(ret == SCE_OK);
}

void MusicPlayer::Terminate()
{
	if (playerHandle != -1)
		Stop();

	auto ret = sceAudiodecTermLibrary(SCE_AUDIODEC_TYPE_M4AAC);
	assert(ret == SCE_OK);
}

void MusicPlayer::Play(void* buffer, size_t size, int sampleRate, int numChannels)
{
	if (playerHandle != -1)
		Stop();

	paramAac.uiSize = sizeof(SceAudiodecParamM4aac);
	paramAac.iBwPcm = SCE_AUDIODEC_WORD_SZ_16BIT;
	paramAac.uiConfigNumber = SCE_AUDIODEC_M4AAC_CONFIG_NUMBER_RAW;
	paramAac.uiSamplingFreqIndex = SCE_AUDIODEC_M4AAC_SAMPLING_FREQ_48000;
	paramAac.uiMaxChannels = 2;
	paramAac.uiEnableHeaac = SCE_AUDIODEC_M4AAC_HEAAC_DISABLE;
	audioDecCtrl.pParam = (void*)&(paramAac);

	aacInfo.uiSize = sizeof(SceAudiodecM4aacInfo);
	aacInfo.uiNumberOfChannels = 2;
	aacInfo.uiSamplingFreq = SCE_AUDIODEC_M4AAC_SAMPLING_FREQ_48000;
	audioDecCtrl.pBsiInfo = (void*)&(aacInfo);

	auInfo.uiSize = sizeof(SceAudiodecAuInfo);
	auInfo.pAuAddr = buffer;
	auInfo.uiAuSize = size;
	audioDecCtrl.pAuInfo = &(auInfo);

	pcmItem.uiSize = sizeof(SceAudiodecPcmItem);
	pcmItem.pPcmAddr = buffer;
	pcmItem.uiPcmSize = size;
	audioDecCtrl.pPcmItem = &(pcmItem);

	auto ret = sceAudiodecCreateDecoder(&audioDecCtrl, SCE_AUDIODEC_TYPE_M4AAC);
	assert(ret == SCE_OK);

	playerHandle = ret;

	playerSampleRate = sampleRate;
	playerNumChannels = numChannels;

	// Decode
	ret = sceAudiodecDecode(playerHandle, &audioDecCtrl);
	assert(ret == 0);
}

void MusicPlayer::Stop()
{
	sceAudiodecDeleteDecoder(playerHandle);
	playerHandle = -1;
}
