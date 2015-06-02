#pragma once

#include "predecls.h"
#include <ngs2.h>

namespace Audio {
		
enum class PlaybackEvent
{
	Play = SCE_NGS2_VOICE_EVENT_PLAY, // 0-Play
	StopAsAuthored = SCE_NGS2_VOICE_EVENT_STOP, // 1-Stop as authored
	StopImmediate = SCE_NGS2_VOICE_EVENT_STOP_IMM, // 2- Stop immediately (Slight fade out)
	Pause = SCE_NGS2_VOICE_EVENT_PAUSE, // 4- Pause
	Resume = SCE_NGS2_VOICE_EVENT_RESUME // 5- Resume
};

class CS_API SoundSystem
{

private:
	
	SoundSystem();

	bool _initialized;

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

	CS_IGNORE void InitVoice(SamplerVoice* voice);
	CS_IGNORE void FreeVoice(SamplerVoice* voice);

	static SoundSystem* GetInstance();

	void Initialize();

	void SubmitPlaybackEvent(SamplerVoice* voiceHandle, AudioBuffer *buffer, PlaybackEvent evt, int portHandle = -1);

	void OpenControllerPort(int playerIdx, uint32_t userID);
	void CloseControllerPort(int playerIdx);

};

} // namespace Audio
