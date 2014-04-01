#include "musicPlayer.h"
#include "../allocator.h"
#include "adts_header_parser.h"
#include "../Audio/soundSystem.h"
#include "../Audio/audioOut.h"

#include <assert.h>
#include <libsysmodule.h>
#include <audiodec.h>
#include <audioout.h>
#include <sceerror.h>

using namespace Audio;
using namespace Media;

bool MusicPlayer::_isInitialized = false;

void loadLibraries()
{
	auto ret = 0;
	if (sceSysmoduleIsLoaded(SCE_SYSMODULE_AUDIODEC) != 0)
	{
		ret = sceSysmoduleLoadModule(SCE_SYSMODULE_AUDIODEC);
		assert(ret == SCE_OK);
	}

	ret = sceAudiodecInitLibrary(SCE_AUDIODEC_TYPE_M4AAC);
	assert(ret == SCE_OK);
}

MusicPlayer::MusicPlayer()
	: _decoder(-1),
	_output(-1)
{

}

MusicPlayer::~MusicPlayer()
{
	Unload();
}

bool MusicPlayer::Load(const char* fileName)
{
	if (!_isInitialized)
	{
		loadLibraries();
		_isInitialized = true;
	}

	assert(fileName != NULL);
	Unload();

	auto file = fopen(fileName, "rb");
	if (file == NULL)
		return false;

	fseek(file, 0, SEEK_END);
	_srcBufferSize = ftell(file);
	_outBufferSize = _srcBufferSize * 4;
	fseek(file, 0, SEEK_SET);
	_srcBuffer = (uint8_t*)Allocator::Get()->allocate(_srcBufferSize);
	_outBuffer = (uint8_t*)Allocator::Get()->allocate(_outBufferSize);
	auto len = fread(_srcBuffer, 1, _srcBufferSize, file);
	assert(len == _srcBufferSize);
	fclose(file);

	AdtsHeaderParser parser;
	const AdtsHeader& header = parser.header();
	auto ret = parser.parse(_srcBuffer, _srcBufferSize);
	assert(ret == SCE_OK);
	_numChannels = header.numChannels;

	SceAudiodecParamM4aac aacParam;
	aacParam.uiSize = sizeof(SceAudiodecParamM4aac);
	aacParam.iBwPcm = SCE_AUDIODEC_WORD_SZ_16BIT;
	aacParam.uiConfigNumber = SCE_AUDIODEC_M4AAC_CONFIG_NUMBER_RAW;
	aacParam.uiSamplingFreqIndex = SCE_AUDIODEC_M4AAC_SAMPLING_FREQ_48000;
	aacParam.uiMaxChannels = header.numChannels;
	aacParam.uiEnableHeaac = SCE_AUDIODEC_M4AAC_HEAAC_DISABLE;

	SceAudiodecM4aacInfo aacInfo;
	aacInfo.uiSize = sizeof(SceAudiodecM4aacInfo);
	aacInfo.uiSamplingFreq = SCE_AUDIODEC_M4AAC_SAMPLING_FREQ_48000;
	aacInfo.uiNumberOfChannels = header.numChannels;
	aacInfo.uiHeaac = SCE_AUDIODEC_M4AAC_HEAAC_DISABLE;
	aacInfo.iResult = 0;

	SceAudiodecAuInfo auInfo;
	auInfo.uiSize = sizeof(SceAudiodecAuInfo);
	auInfo.pAuAddr = _srcBuffer;
	auInfo.uiAuSize = _srcBufferSize;

	SceAudiodecPcmItem pcmItem;
	pcmItem.uiSize = sizeof(SceAudiodecPcmItem);
	pcmItem.pPcmAddr = _outBuffer;
	pcmItem.uiPcmSize = _outBufferSize;

	SceAudiodecCtrl audioDecCtrl;
	audioDecCtrl.pParam = (void*)(&aacParam);
	audioDecCtrl.pBsiInfo = (void*)(&aacInfo);
	audioDecCtrl.pAuInfo = &auInfo;
	audioDecCtrl.pPcmItem = &pcmItem;

	ret = sceAudiodecCreateDecoder(&audioDecCtrl, SCE_AUDIODEC_TYPE_M4AAC);
	if (ret < 0)
		return false;

	_decoder = ret;

	// TODO: Setup streaming instead of a blocking decode load
	ret = sceAudiodecDecode(_decoder, &audioDecCtrl);
	assert(ret == 0);

	// Actual size of decoded data
	_outBufferUsed = auInfo.uiAuSize;

	return true;
}

void MusicPlayer::Unload()
{
	Stop();

	if (_decoder != -1)
	{
		auto ret = sceAudiodecDeleteDecoder(_decoder);
		assert(ret == SCE_OK);
		_decoder = -1;
	}

	if (_outBuffer != NULL)
	{
		Allocator::Get()->release(_outBuffer);
		_outBuffer = NULL;
	}

	if (_srcBuffer != NULL)
	{
		Allocator::Get()->release(_srcBuffer);
		_srcBuffer = NULL;
	}
}

void MusicPlayer::Play()
{
	Stop();
	_output = _audioOut->open(SCE_AUDIO_OUT_PORT_TYPE_BGM, 0);
}

void MusicPlayer::Resume()
{
}

void MusicPlayer::Pause()
{
}

void MusicPlayer::Stop()
{
	if (_output != -1)
	{
		sceAudioOutClose(_output);
		_output = -1;
	}
}

float MusicPlayer::GetVolume()
{
}

void MusicPlayer::SetVolume(float value)
{
}

/*
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
*/
