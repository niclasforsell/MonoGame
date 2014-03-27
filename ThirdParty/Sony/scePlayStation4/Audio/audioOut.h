/* SCE CONFIDENTIAL
PlayStation(R)4 Programmer Tool Runtime Library Release 01.500.111
* Copyright (C) 2012 Sony Computer Entertainment Inc.
* All Rights Reserved.
*/

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <kernel.h>
#include <audioout.h>
#include <user_service.h>
#include "../allocator.h"

namespace Audio {

typedef struct AudioOutBufferInfo{
	float* ptr;
	uint32_t numChannels;
	uint32_t size;
}AudioOutBufferInfo;

typedef int32_t (*AudioOutHandler)(AudioOutBufferInfo *aBufferInfo, uint32_t numInfos, void* arg);

enum {
	AUDIO_OUT_PORT_MAIN=0,
	AUDIO_OUT_PORT_BGM,
	AUDIO_OUT_PORT_PAD0,
	AUDIO_OUT_PORT_PAD1,
	AUDIO_OUT_PORT_PAD2,
	AUDIO_OUT_PORT_PAD3,
	AUDIO_OUT_PORT_PERSONAL0,
	AUDIO_OUT_PORT_PERSONAL1,
	AUDIO_OUT_PORT_PERSONAL2,
	AUDIO_OUT_PORT_PERSONAL3,
	AUDIO_OUT_PORT_MAX
};

#define  AUDIO_OUT_START_FLAG_USE_MAIN_PORT			(1 << 0)
#define  AUDIO_OUT_START_FLAG_USE_BGM_PORT			(1 << 1)
#define  AUDIO_OUT_START_FLAG_USE_PAD0_PORT			(1 << 2)
#define  AUDIO_OUT_START_FLAG_USE_PAD1_PORT			(1 << 3)
#define  AUDIO_OUT_START_FLAG_USE_PAD2_PORT			(1 << 4)
#define  AUDIO_OUT_START_FLAG_USE_PAD3_PORT			(1 << 5)
#define  AUDIO_OUT_START_FLAG_USE_PERSONAL0_PORT	(1 << 6)
#define  AUDIO_OUT_START_FLAG_USE_PERSONAL1_PORT	(1 << 7)
#define  AUDIO_OUT_START_FLAG_USE_PERSONAL2_PORT	(1 << 8)
#define  AUDIO_OUT_START_FLAG_USE_PERSONAL3_PORT	(1 << 9)

class AudioOut
{
public:
	AudioOut(void)
		: m_isCreated(false),
		m_isAborted(false),
		m_thread(0),
		m_sampleRate(0), 
		m_numChannels(0), 
		m_numGrains(0), 
		m_handler(NULL)
	{
	}

	virtual ~AudioOut(void)
	{
		stop();
	}

	// Start audio out service

	int init(
		uint32_t sampleRate,
		uint32_t numChannels,
		uint32_t numGrains,
		AudioOutHandler handler,
		void *arg,
		uint32_t flags)
	{
		int32_t result;
		ScePthreadMutexattr mutexAttr;
		uint32_t index = 0;
		exit();

		m_sampleRate	= sampleRate;
		m_numChannels	= numChannels;
		m_numGrains		= numGrains;
		m_handler		= handler;
		m_arg			= arg;
		m_isAborted		= false;
		m_numPorts      = 0;

		if ((result = sceAudioOutInit()) < 0)
			return result;

		for(int32_t i = 0; i < AUDIO_OUT_PORT_MAX; i++){
			if(flags & (1 << i))
				index++;
		}

		scePthreadMutexattrInit( &mutexAttr );
		if( (result = scePthreadMutexInit( &m_mutex, &mutexAttr, "AudioOutPortMutex" )) < 0){
			scePthreadMutexattrDestroy( &mutexAttr);
			return result;
		}
		scePthreadMutexattrDestroy( &mutexAttr);


		m_aPortInfo = (AudioOutPortInfo*)Allocator::Get()->allocate(index * sizeof(AudioOutPortInfo));
		m_numPorts  = index;
		index = 0;

		for(int32_t i = 0; i < AUDIO_OUT_PORT_MAX; i++){
			if(flags & (1 << i)){
				m_aPortInfo[index].port = i;
				m_aPortInfo[index].userId = -1;
				m_aPortInfo[index].audioOutPortId = -1;
				m_aPortInfo[index].bufferIndex = index;
				switch(i){
				case AUDIO_OUT_PORT_MAIN:
				case AUDIO_OUT_PORT_BGM:
					m_aPortInfo[index].numChannels = numChannels;
					break;
				case AUDIO_OUT_PORT_PAD0:
				case AUDIO_OUT_PORT_PAD1:
				case AUDIO_OUT_PORT_PAD2:
				case AUDIO_OUT_PORT_PAD3:
				case AUDIO_OUT_PORT_PERSONAL0:
				case AUDIO_OUT_PORT_PERSONAL1:
				case AUDIO_OUT_PORT_PERSONAL2:
				case AUDIO_OUT_PORT_PERSONAL3:
					m_aPortInfo[index].numChannels = 1;
					break;
				}
				m_aPortInfo[index].bufferSize  = m_aPortInfo[index].numChannels * numGrains * 2 * sizeof(float);
				m_aPortInfo[index].buffer      = (float*)Allocator::Get()->allocate(m_aPortInfo[index].bufferSize);
				index++;
			}
		}

		m_isCreated = true;

		return 0;
	}

	int start(void)
	{
		if(m_isCreated == false) return 0;

		int result;

		ScePthreadAttr attr;

		//AudioOut Process Thread Create
		scePthreadAttrInit(&attr);
		scePthreadAttrSetstacksize(&attr, 512*1024);

		if ((result = scePthreadCreate( &m_thread, &attr, AudioOut::_process, this, "AudioOutThread")) < 0) {
			scePthreadAttrDestroy(&attr);
			return result;
		}

		scePthreadAttrDestroy(&attr);

		m_isStarted = true;

		return 0;
	}

	int stop(void)
	{
		if(m_isCreated == false || m_isStarted == false) return 0;

		m_isAborted = true;
		scePthreadJoin(m_thread, NULL);

		return 0;
	}

	// Stop audio out service

	int exit(void)
	{
		if(m_isCreated == false) return 0;

		stop();

		for( int i = 0; i < m_numPorts; i++){
			close(i);
			Allocator::Get()->release( m_aPortInfo[i].buffer);
		}

		Allocator::Get()->release(m_aPortInfo);

		m_isCreated = false;

		return 0;
	}

	int open ( int32_t port, int32_t userId)
	{
		int32_t portType, param, portId;
		int32_t aVolume[SCE_NGS2_MAX_VOICE_CHANNELS];

		if( m_isCreated == false ||
			(port < AUDIO_OUT_PORT_MAIN || AUDIO_OUT_PORT_PERSONAL3 < port))
			return -1;

		for(int32_t i = 0; i < m_numPorts; i++){
			if(port == m_aPortInfo[i].port){
				if(m_aPortInfo[i].userId > 0)
					return -1;

				switch(m_aPortInfo[i].numChannels){
				case 1: param = SCE_AUDIO_OUT_PARAM_FORMAT_FLOAT_MONO; break;
				case 2: param = SCE_AUDIO_OUT_PARAM_FORMAT_FLOAT_STEREO; break;
				case 8: param = SCE_AUDIO_OUT_PARAM_FORMAT_FLOAT_8CH; break;
				default: return -1; 
				}

				switch(port){
				case AUDIO_OUT_PORT_MAIN:
					portType = SCE_AUDIO_OUT_PORT_TYPE_MAIN;
					userId   = SCE_USER_SERVICE_USER_ID_SYSTEM;
					break;
				case AUDIO_OUT_PORT_BGM:
					portType = SCE_AUDIO_OUT_PORT_TYPE_BGM;
					userId   = SCE_USER_SERVICE_USER_ID_SYSTEM;
					break;
				case AUDIO_OUT_PORT_PAD0:
				case AUDIO_OUT_PORT_PAD1:
				case AUDIO_OUT_PORT_PAD2:
				case AUDIO_OUT_PORT_PAD3:
					portType = SCE_AUDIO_OUT_PORT_TYPE_PADSPK;
					break;
				case AUDIO_OUT_PORT_PERSONAL0:
				case AUDIO_OUT_PORT_PERSONAL1:
				case AUDIO_OUT_PORT_PERSONAL2:
				case AUDIO_OUT_PORT_PERSONAL3:
					portType = SCE_AUDIO_OUT_PORT_TYPE_PERSONAL;
					break;
				}

				portId = sceAudioOutOpen(
					userId,
					portType,
					0,
					m_numGrains,
					m_sampleRate,
					param);

				for(int j = 0; j < m_aPortInfo[i].numChannels; j++){
					aVolume[j] = SCE_AUDIO_VOLUME_0dB;
				}

				switch(m_aPortInfo[i].numChannels){
				case 1:
					sceAudioOutSetVolume(
						portId,
						SCE_AUDIO_VOLUME_FLAG_FL_CH,
						aVolume);
					break;
				case 2:
					sceAudioOutSetVolume(
						portId,
						SCE_AUDIO_VOLUME_FLAG_FL_CH  | SCE_AUDIO_VOLUME_FLAG_FR_CH,
						aVolume);
					break;
				case 8:
					sceAudioOutSetVolume(
						portId,
						SCE_AUDIO_VOLUME_FLAG_FL_CH  | SCE_AUDIO_VOLUME_FLAG_FR_CH
						| SCE_AUDIO_VOLUME_FLAG_CNT_CH | SCE_AUDIO_VOLUME_FLAG_LFE_CH 
						| SCE_AUDIO_VOLUME_FLAG_RL_CH  | SCE_AUDIO_VOLUME_FLAG_RR_CH
						| SCE_AUDIO_VOLUME_FLAG_BL_CH  | SCE_AUDIO_VOLUME_FLAG_BR_CH,
						aVolume);
					break;
				}

				scePthreadMutexLock  ( &m_mutex );
				m_aPortInfo[i].userId		  = userId;
				m_aPortInfo[i].audioOutPortId = portId;
				scePthreadMutexUnlock( &m_mutex );

				return i;
			}
		}

		return -1;
	}

	int close( int32_t id)
	{
		int32_t portId;

		if( m_isCreated == false ||
			(id < 0 || m_numPorts < id))
			return -1;

		if(m_aPortInfo[id].userId < 0)
			return -1;

		scePthreadMutexLock( &m_mutex );

		portId = m_aPortInfo[id].audioOutPortId;

		m_aPortInfo[id].audioOutPortId = -1;
		m_aPortInfo[id].userId		   = -1;

		scePthreadMutexUnlock( &m_mutex );

		sceAudioOutOutput( portId, NULL);
		sceAudioOutClose( portId );


		return 0;
	}

	int getBufferId( int32_t id)
	{
		if( m_isCreated == false ||
			(id < 0 || m_numPorts < id))
			return -1;

		return m_aPortInfo[id].bufferIndex;
	}

	int getNumPorts(void)
	{
		if( m_isCreated == false )
			return -1;

		return m_numPorts;
	}

private:

	// Process

	static void *_process(void* arg)
	{
		AudioOut* audioOut = (AudioOut*)arg;
		audioOut->process();
		return NULL;
	}

	int process(void)
	{
		uint32_t bufferId;

		// Output loop

		bufferId = 0;

		while (m_isAborted == false) {
			AudioOutBufferInfo aInfo[AUDIO_OUT_PORT_MAX];
			SceAudioOutOutputParam aOutputParam[AUDIO_OUT_PORT_MAX];
			int32_t numOutputs = 0;

			for(int i = 0; i < m_numPorts; i++){
				aInfo[i].ptr         = m_aPortInfo[i].buffer + bufferId * m_numGrains * m_aPortInfo[i].numChannels;
				aInfo[i].size        = m_numGrains * m_aPortInfo[i].numChannels * sizeof(float);
				aInfo[i].numChannels = m_aPortInfo[i].numChannels;
			}

			// Call user rendering handler

			(*m_handler)(aInfo, m_numPorts, m_arg);

			scePthreadMutexLock( &m_mutex );

			for( int i = 0; i < m_numPorts; i++){
				if(m_aPortInfo[i].userId > 0){
					aOutputParam[numOutputs].ptr    = aInfo[i].ptr;
					aOutputParam[numOutputs].handle = m_aPortInfo[i].audioOutPortId;
					numOutputs++;
				}
			}

			// Output
			if(numOutputs > 0){
				sceAudioOutOutputs( aOutputParam, numOutputs);
			}

			scePthreadMutexUnlock( &m_mutex );

			// Swap buffer

			bufferId ^= 1;
		}

		return 0;
	}

private:
	typedef struct AudioOutPortInfo{
		int32_t					port;
		SceUserServiceUserId	userId;
		int32_t					audioOutPortId;
		uint32_t				numChannels;
		uint32_t				bufferIndex;
		float*					buffer;
		uint32_t				bufferSize;
	}AudioOutPortInfo;


	bool m_isCreated;				// Is created ?
	bool m_isStarted;				// Is started ?
	volatile bool m_isAborted;		// Is aborted ?
	ScePthread m_thread;			// Pthread
	uint32_t m_sampleRate;			// Sampling rate [Hz]
	uint32_t m_numChannels;			// Number of channels
	uint32_t m_numGrains;			// Number of grains
	AudioOutHandler m_handler;		// Rendering handler
	void	*m_arg;					// argument to handler
	AudioOutPortInfo *m_aPortInfo;	// port Info;
	uint32_t m_numPorts;
	ScePthreadMutex	m_mutex;
};

} // namespace Audio
