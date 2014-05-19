#include "SamplerVoice.h"
#include "SoundSystem.h"
#include "AudioBuffer.h"
#include <assert.h>
#include <math.h>
#include <iostream>

using namespace Audio;

SamplerVoice::SamplerVoice(unsigned int id, SceNgs2Handle rackHandle, SceNgs2Handle voiceHandle, AudioBuffer* buffer) :
	_voiceHandle(voiceHandle),
	_rackHandle(rackHandle),
	_voiceHandleID(id),
	_pitch(0.0f),
	_looped(false),
	_volume(1.0f),
	_buffer(buffer)
{
}

SoundState SamplerVoice::GetState()
{
	uint32_t stateFlags;

	sceNgs2RackGetVoiceHandle(_rackHandle, _voiceHandleID, &_voiceHandle);
	int errorCode = sceNgs2VoiceGetStateFlags(_voiceHandle, &stateFlags);

	assert(errorCode >= 0);

	if(stateFlags == 0)
		return SoundState::Stopped;

	if (stateFlags & SCE_NGS2_VOICE_STATE_FLAG_STOPPED ||
		stateFlags >= SCE_NGS2_VOICE_STATE_FLAG_ERROR)
		return SoundState::Stopped;

	if (stateFlags & SCE_NGS2_VOICE_STATE_FLAG_PLAYING ||
		stateFlags == SCE_NGS2_VOICE_STATE_FLAG_INUSE)
		return SoundState::Playing;

	if (stateFlags & SCE_NGS2_VOICE_STATE_FLAG_PAUSED)
		return SoundState::Paused;

	//Unknown state
	assert(false);
}

bool SamplerVoice::GetLooped()
{
	return _looped;
}

void SamplerVoice::SetLooped(bool loop)
{
	_looped = loop;

	auto errorCode = sceNgs2RackGetVoiceHandle(_rackHandle, _voiceHandleID, &_voiceHandle);
	assert(errorCode == 0);

	auto waveformInfo = _looped ? _buffer->_loopedWaveformInfo : _buffer->_waveformInfo;

	SceNgs2SamplerVoiceSetupParam param;
	param.flags = SCE_NGS2_SAMPLER_VOICE_ADD_WAVEFORM_BLOCKS;

	errorCode = sceNgs2SamplerVoiceSetupParam(&param.header, 0, &waveformInfo->format, 0);
	assert(errorCode == 0);
	errorCode = sceNgs2VoiceControl(_voiceHandle, &param.header);
	assert(errorCode == 0);
	errorCode = sceNgs2SamplerVoiceAddWaveformBlocks(_voiceHandle, _buffer->_waveformData, waveformInfo->aBlock, waveformInfo->numBlocks, 0);
	assert(errorCode == 0);
}

void SamplerVoice::SetFade(float fadeinTime, float fadeOutTime)
{
	auto fadeInPts = 0;
	auto fadeOutPts = 0;
	auto sampleRate = _buffer->_waveformInfo->format.sampleRate;
	SceNgs2EnvelopePoint envelopePoints[2];

	if (fadeinTime > 0)
	{
		envelopePoints[0].curve = SCE_NGS2_ENVELOPE_CURVE_LINEAR;
		envelopePoints[0].height = 0;
		envelopePoints[0].duration = 0;
		envelopePoints[1].curve = SCE_NGS2_ENVELOPE_CURVE_LINEAR;
		envelopePoints[1].height = 1;
		envelopePoints[1].duration = sampleRate * fadeinTime;
		fadeInPts = 2;
	}
	else if (fadeOutTime > 0)
	{
		envelopePoints[0].curve = SCE_NGS2_ENVELOPE_CURVE_LINEAR;
		envelopePoints[0].height = 1;
		envelopePoints[0].duration = 0;
		envelopePoints[1].curve = SCE_NGS2_ENVELOPE_CURVE_LINEAR;
		envelopePoints[1].height = 0;
		envelopePoints[1].duration = sampleRate * fadeOutTime;
		fadeOutPts = 2;
	}
	else
	{
		// If we're not fading in or out then 
		// we need to clear the envelope.
		envelopePoints[0].curve = SCE_NGS2_ENVELOPE_CURVE_LINEAR;
		envelopePoints[0].height = 1;
		envelopePoints[0].duration = 0;
		envelopePoints[1].curve = SCE_NGS2_ENVELOPE_CURVE_LINEAR;
		envelopePoints[1].height = 1;
		envelopePoints[1].duration = 0;
		fadeInPts = fadeOutPts = 1;
	}

	auto errorCode = sceNgs2SamplerVoiceSetEnvelope(_voiceHandle, envelopePoints, fadeInPts, fadeOutPts);
	assert(errorCode == 0);
}

float SamplerVoice::GetVolume()
{
	return _volume;
}

void SamplerVoice::SetVolume(float newVol)
{
	_volume = newVol;

	sceNgs2RackGetVoiceHandle(_rackHandle, _voiceHandleID, &_voiceHandle);

	auto errorCode = sceNgs2VoiceSetPortVolume(_voiceHandle, 0, newVol);
	assert(errorCode >= 0);
}

void SamplerVoice::SetPan(float pan)
{
	_pan = pan;

	// PS4 Sets defines its pan values between 0-360 degrees. 0 is front/center,
	// 90 is to the right, 180 is rear and 270 is to the left.
	// Scale out -1 - 1 range between 270 and 90.
	auto panMagnitude = abs(pan);

	auto panAmount = 90.0f * panMagnitude;

	// If we are setting this to the left of center, wrap the angle
	if(pan < 0.0f)
		panAmount = 360.0f -  panAmount;

	// Calculate our new panned volume matrix
	// and push it into the system.

	auto params = SceNgs2PanParam();
	params.angle = panAmount;

	auto panWork = SceNgs2PanWork();
	panWork.numSpeakers = 2;
	panWork.aSpeakerAngle[0] = 270;
	panWork.aSpeakerAngle[1] = 90;
	panWork.unitAngle = SCE_NGS2_PAN_ANGLE_DEGREE;

	static float levelArray[2] = { 1.0f, 1.0f };
	auto errorCode = sceNgs2PanGetVolumeMatrix(&panWork, &params, 1, SCE_NGS2_PAN_MATRIX_FORMAT_2_0CH, levelArray);
	assert(errorCode >= 0);

	sceNgs2RackGetVoiceHandle(_rackHandle, _voiceHandleID, &_voiceHandle);
	errorCode = sceNgs2VoiceSetMatrixLevels(_voiceHandle, 0, levelArray, 2);
	assert(errorCode >= 0);
}

float SamplerVoice::GetPan()
{
	return _pan;
}

void SamplerVoice::SetPitch(float pitch)
{
	sceNgs2RackGetVoiceHandle(_rackHandle, _voiceHandleID, &_voiceHandle);
	auto errorCode = sceNgs2SamplerVoiceSetPitch(_voiceHandle, pitch);

	assert(errorCode >= 0);
}

float SamplerVoice::GetPitch()
{
    // NOTE: This is copy of what XAudio2.FrequencyRatioToSemitones()
	auto semitones = 39.86313713864835f * log10(_pitch);

    // Convert from semitones to octaves.
    semitones /= 12.0f;

    return semitones;
}

SamplerVoice::~SamplerVoice(void)
{
	SoundSystem::GetInstance()->DestroyVoice(this);

	_voiceHandleID = 0;
}
