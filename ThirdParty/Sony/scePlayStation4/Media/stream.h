/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2013 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#ifndef _STREAM_H
#define _STREAM_H

#include <stdint.h>
#include "ring_buffer.h"
#include "file.h"
#include "audio_output.h"

namespace Media {

class Stream
{
public:
	Stream(uint32_t size, uint32_t sizeHead) : m_buffer(size, sizeHead) {}
	virtual ~Stream(void) {}
	virtual int open(const char *path, const char *mode) = 0;
	virtual int open(int32_t grain, int32_t sampleRate, int32_t param) = 0;
	virtual int close(void) = 0;
	virtual uint32_t size(void) { return 0; }
	uint32_t bufferSize(void) { return m_buffer.size(); }
	bool isEmpty(void) { return m_buffer.isEmpty(); }
	int end(void) { return m_buffer.end(); }
protected:
	RingBuffer m_buffer;
};

class InputStream : public Stream
{
public:
	InputStream(uint32_t size, uint32_t sizeHead) : Stream(size, sizeHead) {}
	virtual ~InputStream(void) {}
	virtual int input(uint32_t size = 0) = 0;
	const uint8_t *lock(uint32_t size = 0) { return m_buffer.lockReadingRoom(size); }
	int unlock(uint32_t size) { return m_buffer.unlockReadingRoom(size); }
	uint32_t readingSize(void) { return m_buffer.readingSize(); }
};

class OutputStream : public Stream
{
public:
	OutputStream(uint32_t size, uint32_t sizeHead) : Stream(size, sizeHead) {}
	virtual ~OutputStream(void) {}
	virtual int output(uint32_t size = 0) = 0;
	uint8_t *lock(uint32_t size = 0) { return m_buffer.lockWritingRoom(size); }
	int unlock(uint32_t size) { return m_buffer.unlockWritingRoom(size); }
	uint32_t writingSize(void) { return m_buffer.writingSize(); }
};

class FileInputStream : public InputStream
{
public:
	FileInputStream(uint32_t size, uint32_t sizeHead = 0) : InputStream(size, sizeHead) {}
	~FileInputStream(void) {}
	int open(const char *path, const char *mode) { return m_file.open(path, mode); }
	int open(int32_t grain, int32_t sampleRate, int32_t param) { return -1; }
	int close(void) { return m_file.close(); }
	int input(uint32_t size = 0);
	uint32_t size(void) { return m_file.size(); }
private:
	File m_file;
};

class FileOutputStream : public OutputStream
{
public:
	FileOutputStream(uint32_t size, uint32_t sizeHead = 0) : OutputStream(size, sizeHead) {}
	~FileOutputStream(void) {}
	int open(const char *path, const char *mode) { return m_file.open(path, mode); }
	int open(int32_t grain, int32_t sampleRate, int32_t param) { return -1; }
	int close(void) { return m_file.close(); }
	int output(uint32_t size = 0);
	uint32_t size(void) { return m_file.size(); }
private:
	File m_file;
};

class AudioOutputStream : public OutputStream
{
public:
	AudioOutputStream(uint32_t size, uint32_t sizeHead = 0) : OutputStream(size, sizeHead), m_hasOutput(false) {}
	~AudioOutputStream(void) {}
	int open(const char *path, const char *mode) { return -1; }
	int open(int32_t grain, int32_t sampleRate, int32_t param) { return m_audioOutput.open(grain, sampleRate, param); }
	int close(void)
	{
		m_hasOutput = false;
		return m_audioOutput.close();
	}
	int output(uint32_t size = 0);
	uint32_t size(void) { return m_audioOutput.size(); }
	AudioOutput* getOutput() { return &m_audioOutput; }
private:
	AudioOutput m_audioOutput;
	bool m_hasOutput;
};

} // namespace Media

#endif /* _STREAM_H */
