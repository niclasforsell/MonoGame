#pragma once

#include "predecls.h"
#include <ngs2.h>

namespace Audio {

class __declspec(dllexport) SoundSystem
{

private:
	
	SoundSystem(void);

	unsigned int _currentVoiceHandle;

	SceNgs2Handle _systemHandle;			// NGS2 system handle
	SceNgs2Handle _masteringRackHandle;	// Mastering rack handle
	SceNgs2Handle _samplerRackHandle;		// Sample rack handle
	SceNgs2Handle _masteringVoiceHandle;	// Mastering voice handle
	SceNgs2Handle _masteringPadVoiceHandle[4];	// Pad Mastering voice handle

	AudioOut *m_audioOut;					// Audio out service

	int m_padPort[4];

	void *_freeVoiceIDs;

	static int32_t _audioOutMain(AudioOutBufferInfo* aBufferInfo, uint32_t numInfos, void* arg);

	int32_t audioOutMain(AudioOutBufferInfo* aBufferInfo, uint32_t numInfos);

public:

	const unsigned int MAX_VOICES = 1024;

	void DestroyVoice(SamplerVoice* voice);
	SamplerVoice* CreateVoice(AudioBuffer* buffer);

	static SoundSystem* Instance();

	void Initialize();

	void SubmitPlaybackEvent(SamplerVoice* voiceHandle, AudioBuffer *buffer, int evt, int portHandle = -1);

	void OpenControllerPort(int playerIdx, uint32_t userID);
	void CloseControllerPort(int playerIdx);

	~SoundSystem(void);
};

} // namespace Audio
