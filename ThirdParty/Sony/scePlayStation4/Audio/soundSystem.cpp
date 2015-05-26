#include "soundSystem.h"
#include "audioOut.h"
#include "audioBuffer.h"
#include "samplerVoice.h"
#include "../allocator.h"

#include <ngs2.h>
#include <libsysmodule.h>
#include <sulpha.h>
#include <assert.h>
#include <stdlib.h>
#include <queue>

using namespace Audio;

#if _DEBUG
//#define SOUND_LOGGING 1
#endif

SoundSystem* SoundSystem::GetInstance()
{
	static SoundSystem instance;

	return &instance;
}

static int32_t bufferAlloc(SceNgs2ContextBufferInfo *bufferInfo)
{
	bufferInfo->hostBuffer = mem::alloc(bufferInfo->hostBufferSize);
#if SOUND_LOGGING
	printf("SoundSystem Allocate (%p,%zd[byte])\n", bufferInfo->hostBuffer, bufferInfo->hostBufferSize);
#endif
	return (bufferInfo->hostBuffer)? SCE_OK : SCE_NGS2_ERROR_EMPTY_BUFFER;
}
	
static int32_t bufferFree(SceNgs2ContextBufferInfo *bufferInfo)
{
#if SOUND_LOGGING
	printf("SoundSystem Free (%p,%zd[byte])\n", bufferInfo->hostBuffer, bufferInfo->hostBufferSize);
#endif
	mem::free(bufferInfo->hostBuffer);
	return SCE_OK;
}

int32_t SoundSystem::_audioOutMain(AudioOutBufferInfo* aBufferInfo, uint32_t numInfos, void* arg)
{
	return ((SoundSystem*)arg)->audioOutMain(aBufferInfo, numInfos);
}

int32_t SoundSystem::audioOutMain(AudioOutBufferInfo* aBufferInfo, uint32_t numInfos)
{
	SceNgs2RenderBufferInfo aInfo[AUDIO_OUT_PORT_MAX];
	int32_t result;

	// Rendering

	for(int i = 0; i < numInfos; i++)
	{
		aInfo[i].buffer       = aBufferInfo[i].ptr;
		aInfo[i].bufferSize   = aBufferInfo[i].size;
		aInfo[i].numChannels  = aBufferInfo[i].numChannels;
		aInfo[i].waveformType = SCE_NGS2_WAVEFORM_TYPE_PCM_F32L;
	}

	result = sceNgs2SystemRender(_systemHandle, aInfo, numInfos);

	assert(result >= 0);

	return 0;
}

SoundSystem::SoundSystem(void) :
	_initialized(false)
{
	_freeVoiceIDs = new std::queue<unsigned int>();
}

void SoundSystem::Initialize()
{
	if (_initialized)
		return;

	int errorCode;

#ifdef _DEBUG

	{
		errorCode = sceSysmoduleLoadModule( SCE_SYSMODULE_SULPHA );
		assert(errorCode == 0);
		SceSulphaConfig config;
		sceSulphaGetDefaultConfig(&config);
		size_t size;
		sceSulphaGetNeededMemory(&config, &size);
		auto sulphaMem = mem::alloc(size);
		sceSulphaInit(&config, sulphaMem, size);
	}

#endif

	// Initialize padPort array.
	for(auto x = 0; x < 4; x++)
		m_padPort[x] = -1;
	
	SceNgs2BufferAllocator allocator;
	
	errorCode = sceSysmoduleLoadModule( SCE_SYSMODULE_NGS2 );
	assert(errorCode == 0);

	auto flags = AUDIO_OUT_START_FLAG_USE_MAIN_PORT |
				 AUDIO_OUT_START_FLAG_USE_PAD0_PORT |
				 AUDIO_OUT_START_FLAG_USE_PAD1_PORT |
				 AUDIO_OUT_START_FLAG_USE_PAD2_PORT |
				 AUDIO_OUT_START_FLAG_USE_PAD3_PORT;

	// Start audio out service
	m_audioOut = new AudioOut();
	errorCode = m_audioOut->init(
			48000,	// [Hz]
			8,		// [ch]
			256,	// [samples]
			SoundSystem::_audioOutMain,
			this,
			flags);

	// Create NGS2 system, racks

	allocator.allocHandler = bufferAlloc;
	allocator.freeHandler  = bufferFree;
	allocator.userData     = 0;

	auto mainPort = m_audioOut->open( AUDIO_OUT_PORT_MAIN, 0 );

	errorCode = sceNgs2SystemCreateWithAllocator(NULL, &allocator, &_systemHandle);
	assert(errorCode == 0);

	// Setup the mastering rack.
	SceNgs2MasteringRackOption masteringOption;
	sceNgs2MasteringRackResetOption(&masteringOption);
	masteringOption.rackOption.maxVoices = m_audioOut->getNumPorts();
#if _DEBUG
	strcpy(masteringOption.rackOption.name, "master_rack");
	masteringOption.rackOption.flags = SCE_NGS2_RACK_OPTION_FLAG_DIAG;
#endif

	errorCode = sceNgs2RackCreateWithAllocator(_systemHandle, SCE_NGS2_RACK_ID_MASTERING, &masteringOption.rackOption, &allocator, &_masteringRackHandle);
	assert(errorCode == 0);

	// Setup the sampler rack.
	SceNgs2SamplerRackOption samplerOption;
	sceNgs2SamplerRackResetOption(&samplerOption);
	samplerOption.rackOption.maxVoices = MAX_VOICES;
	samplerOption.maxChannelWorks = MAX_VOICES;
#if _DEBUG
	strcpy(samplerOption.rackOption.name, "sampler_rack");
	samplerOption.rackOption.flags = SCE_NGS2_RACK_OPTION_FLAG_DIAG;
#endif

	errorCode = sceNgs2RackCreateWithAllocator(_systemHandle, SCE_NGS2_RACK_ID_SAMPLER, &samplerOption.rackOption, &allocator, &_samplerRackHandle);
	assert(errorCode == 0);

	errorCode = sceNgs2RackGetVoiceHandle(_masteringRackHandle, 0, &_masteringVoiceHandle);
	assert(errorCode == 0);

	errorCode = sceNgs2MasteringVoiceSetup(_masteringVoiceHandle,SCE_NGS2_CHANNELS_7_1CH, 0);
	assert(errorCode == 0);

	errorCode = sceNgs2MasteringVoiceSetOutput( _masteringVoiceHandle, m_audioOut->getBufferId( mainPort ));
	assert(errorCode == 0);

	errorCode = sceNgs2VoiceKickEvent(_masteringVoiceHandle, SCE_NGS2_VOICE_EVENT_PLAY);
	assert(errorCode == 0);

	errorCode = m_audioOut->start();

	assert(errorCode == 0);

	auto ids = ((std::queue<unsigned int>*)_freeVoiceIDs);
	for (auto x = 0; x < MAX_VOICES; x++)
		ids->push(x);

	for (auto x = 0; x < 4; x++)
		_masteringPadVoiceHandle[x] = 0;

	_initialized = true;
}

void SoundSystem::SubmitPlaybackEvent(SamplerVoice* voiceHandle, AudioBuffer *buffer, PlaybackEvent evt, int portHandle)
{
	// TODO: It appears that all of this is necessary before playing a sound
	// ... Should confirm that there's not a way to do this by saving the handle.

	SceNgs2Handle handle;
	sceNgs2RackGetVoiceHandle(_samplerRackHandle, voiceHandle->_voiceHandleID, &handle);

	int errorCode = 0;

	// If we're about to play a paused or stopped sound,
	// ensure that the waveform data and info is current.
	// Stop and pause can just happen immediately without any voice 
	// configuration.
	if (evt == PlaybackEvent::Play || evt == PlaybackEvent::Resume)
	{
		uint32_t stateFlags = SCE_NGS2_VOICE_STATE_FLAG_ERROR;
		errorCode = sceNgs2VoiceGetStateFlags(handle, &stateFlags);
		assert(errorCode >= 0);

		// In XNA, Play can be used to resume a paused sound 
		// as well as resume. This isn't the case on PS4, so 
		// always ensure we're performing the correct operation.
		if (stateFlags & SCE_NGS2_VOICE_STATE_FLAG_PAUSED)
			evt = PlaybackEvent::Resume;
		else
			evt = PlaybackEvent::Play;

		if (portHandle == -1 && _masteringPadVoiceHandle[portHandle] != 0)
			errorCode = sceNgs2VoicePatch(handle, 0, _masteringVoiceHandle, 0);
		else
			errorCode = sceNgs2VoicePatch(handle, 0, _masteringPadVoiceHandle[portHandle], 0);

		assert(errorCode >= 0);
	}

	errorCode = sceNgs2VoiceKickEvent(handle, (int)evt);

	assert(errorCode >= 0);
}

void SoundSystem::InitVoice(SamplerVoice* voice)
{
	auto ids = ((std::queue<unsigned int>*)_freeVoiceIDs);

	if (ids->size() == 0)
	{
		printf("WARNING: Couldn't find a free voice ID to use.\n");
		assert(!"SoundSystem::CreateVoice - Ran out of voices!");
		return;
	}

	SceNgs2Handle voiceHandle;
	int errorCode;

	auto nextID = ids->front();
	ids->pop();

#if SOUND_LOGGING
	printf("SoundSystem::CreateVoice: %d voices free.\n", ids->size());
#endif

	errorCode = sceNgs2RackGetVoiceHandle(_samplerRackHandle, nextID, &voiceHandle);
	assert(errorCode >= 0);

	auto buffer = voice->_buffer;

	auto waveformInfo = buffer->_waveformInfo;

	errorCode = sceNgs2SamplerVoiceSetup(voiceHandle, &waveformInfo->format, 0);

	assert(errorCode >= 0);
	errorCode = sceNgs2SamplerVoiceAddWaveformBlocks(voiceHandle, buffer->_waveformData, waveformInfo->aBlock, waveformInfo->numBlocks, 0);

	voice->_voiceHandleID = nextID;
	voice->_voiceHandle = voiceHandle;
	voice->_rackHandle = _samplerRackHandle;
}

void SoundSystem::FreeVoice(SamplerVoice* voice)
{
	if (voice->GetState() != SoundState::Stopped)
		SubmitPlaybackEvent(voice, NULL, PlaybackEvent::StopImmediate);

	auto ids = ((std::queue<unsigned int>*)_freeVoiceIDs);
	ids->push(voice->_voiceHandleID);
	voice->_voiceHandleID = 0;

#if SOUND_LOGGING
	printf("SoundSystem::DestroyVoice: %d voices free.\n", ids->size());
#endif
}

void SoundSystem::OpenControllerPort(int playerIdx, uint32_t userID)
{
	assert(m_padPort[playerIdx] == -1);

	m_padPort[playerIdx] = m_audioOut->open( AUDIO_OUT_PORT_PAD0 + playerIdx, userID);

	auto errorCode = sceNgs2RackGetVoiceHandle(_masteringRackHandle, 1 + playerIdx, &_masteringPadVoiceHandle[playerIdx]);
	assert(errorCode == 0);

	errorCode = sceNgs2MasteringVoiceSetup(_masteringPadVoiceHandle[playerIdx], SCE_NGS2_CHANNELS_1_0CH, 0);
	assert(errorCode == 0);
	errorCode = sceNgs2MasteringVoiceSetOutput( _masteringPadVoiceHandle[playerIdx], m_audioOut->getBufferId( m_padPort[playerIdx] ));
	assert(errorCode == 0);
	errorCode = sceNgs2VoiceKickEvent(_masteringPadVoiceHandle[playerIdx], SCE_NGS2_VOICE_EVENT_PLAY);
	assert(errorCode == 0);
}

void SoundSystem::CloseControllerPort(int playerIdx)
{
	m_audioOut->close(m_padPort[playerIdx]);

	m_padPort[playerIdx] = -1;
	_masteringPadVoiceHandle[playerIdx] = 0;
}

