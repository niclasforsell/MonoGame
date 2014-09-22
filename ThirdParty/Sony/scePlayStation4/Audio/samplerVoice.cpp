#include "SamplerVoice.h"
#include "SoundSystem.h"
#include "AudioBuffer.h"
#include <assert.h>
#include <math.h>
#include <iostream>

using namespace Audio;

SamplerVoice::SamplerVoice(unsigned int id, SceNgs2Handle rackHandle, SceNgs2Handle voiceHandle, AudioBuffer* buffer) :
	_rackHandle(rackHandle),
	_voiceHandle(voiceHandle),
	_voiceHandleID(id),
	_pitch(0.0f),
	_pan(0.0f),
	_volume(1.0f),
	_looped(false),
	_buffer(buffer)
{
}

SoundState SamplerVoice::GetState()
{
	uint32_t stateFlags;

	auto errorCode = sceNgs2VoiceGetStateFlags(_voiceHandle, &stateFlags);

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

	auto waveformInfo = _looped ? _buffer->_loopedWaveformInfo : _buffer->_waveformInfo;

	SceNgs2SamplerVoiceSetupParam param;
	param.flags = SCE_NGS2_SAMPLER_VOICE_ADD_WAVEFORM_BLOCKS;

	auto errorCode = sceNgs2SamplerVoiceSetupParam(&param.header, 0, &waveformInfo->format, 0);
	assert(errorCode == 0);
	errorCode = sceNgs2VoiceControl(_voiceHandle, &param.header);
	assert(errorCode == 0);
	errorCode = sceNgs2SamplerVoiceAddWaveformBlocks(_voiceHandle, _buffer->_waveformData, waveformInfo->aBlock, waveformInfo->numBlocks, 0);
	assert(errorCode == 0);

	// samplerVoiceSetupParam/AddWaveFormBlocks will stomp over state.
	// Update the Volume/Pan/Pitch so they aren't overwritten.
	SetVolume(_volume);
	SetPitch(_pitch);
	SetPan(_pan);
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

	auto errorCode = sceNgs2VoiceSetPortVolume(_voiceHandle, 0, newVol);
	assert(errorCode >= 0);
}

void SamplerVoice::SetPan(float pan)
{
	_pan = pan;

	// Convert the -1 to 1 pan to 0 to 360 panning. 
	//
	// 0 is front
	// 90 is right
	// 180 is rear
	// 270 is left

	auto panAmount = 90.0f * abs(pan);
	if (pan < 0.0f)
		panAmount = 360.0f -  panAmount;

	// Calculate our panned volume levels for each output channel.
	float panLevels[SCE_NGS2_MAX_VOICE_CHANNELS] = {0};
	GetPanLevels(panAmount, panLevels);

	// Apply the panning levels to the output matrix.
	SetMatrixLevels(panLevels);
}

void SamplerVoice::GetPanLevels(float angle, float *panLevels)
{
	auto params = SceNgs2PanParam();
	params.angle = angle;
	params.distance  = 1.0f;
	params.lfeLevel  = 1.0f; // Low frequency effects (subwoofer) levels.
	params.fbwLevel  = 1.0f; // Full band width (the speakers that are not subwoofers) levels.

	SceNgs2PanWork panWork;
	auto errorCode = sceNgs2PanInit( &panWork, NULL, SCE_NGS2_PAN_ANGLE_DEGREE, SCE_NGS2_PAN_SPEAKER_7_0CH);
	assert(errorCode >= 0);

	errorCode = sceNgs2PanGetVolumeMatrix(&panWork, &params, 1, SCE_NGS2_PAN_MATRIX_FORMAT_7_1CH, panLevels);
	assert(errorCode >= 0);
}

void SamplerVoice::SetMatrixLevels(float *panLevels)
{
	// Is this a 1ch or a 2ch sound.
	auto bufferChannels = _buffer->_waveformInfo->format.numChannels;

	// Interleave the final levels depending on the number of input channels.
	float matrixLevels[SCE_NGS2_MAX_VOICE_CHANNELS * 2] = {0};
	for (auto p=0; p < SCE_NGS2_MAX_VOICE_CHANNELS; p++)
	{
		for (auto c=0; c < bufferChannels; c++)
			matrixLevels[(p * bufferChannels) + c] = panLevels[p]; 
	}

	auto errorCode = sceNgs2VoiceSetMatrixLevels(_voiceHandle, 0, matrixLevels, SCE_NGS2_MAX_VOICE_CHANNELS * bufferChannels);
	assert(errorCode >= 0);
	errorCode = sceNgs2VoiceSetPortMatrix(_voiceHandle, 0, 0);
	assert(errorCode >= 0);
}

float SamplerVoice::GetPan()
{
	return _pan;
}

void SamplerVoice::SetPitch(float pitch)
{
	_pitch = pitch;

	// Convert from XNA octive ratio of -1, 0, 1 to 
	// the Ngs2 pitch scale of 0, 1, 2.
	float pitchRatio = powf(2, pitch);

	auto errorCode = sceNgs2SamplerVoiceSetPitch(_voiceHandle, pitchRatio);

	assert(errorCode >= 0);
}

float SamplerVoice::GetPitch()
{
    return _pitch;
}

SamplerVoice::~SamplerVoice(void)
{
	SoundSystem::GetInstance()->DestroyVoice(this);
}
