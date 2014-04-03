/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2012 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#ifndef _AUDIO_OUTPUT_H
#define _AUDIO_OUTPUT_H

#include <stdint.h>

namespace Media {

#define AUDIO_MONAURAL (1)
#define AUDIO_STEREO   (2)
#define AUDIO_SURROUND (8)

class AudioOutputSystem
{
public:
	AudioOutputSystem(void);
	~AudioOutputSystem(void) {}
	static int32_t param(uint32_t numChannels);
	static uint32_t numChannels(int32_t param);
};

class AudioOutput
{
public:
	AudioOutput(void);
	~AudioOutput(void);
	int open(int32_t grain, int32_t sampleRate, int32_t param);
	int close(void);
	int output(const void *p);
	uint32_t size(void) { return sizeof(int16_t) * m_numChannels * m_grain; }
	float getVolume();
	void setVolume(float value);
private:
	static AudioOutputSystem s_system;
	int32_t m_handle;
	int32_t m_grain;
	int32_t m_sampleRate;
	int32_t m_param;
	uint32_t m_numChannels;
	int32_t m_volume[AUDIO_STEREO];
};

} // namespace Media

#endif /* _AUDIO_OUTPUT_H */
