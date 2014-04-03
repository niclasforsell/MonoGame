/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2013 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#include <stdio.h>
#include "stream.h"

using namespace Media;

int InputStream::input(char *pInputBuf, uint32_t size, off_t offset, int32_t whence)
{
	int readSize = 0;
	char tmp;

	if (pInputBuf == NULL) {
		if (size == 0) {
			// control file point
			readSize = m_file.read(&tmp, size, offset, whence);
		} else {
			printf("error: File::read() Buffer Error\n");
			return -1;
		}
	} else {
		// read file data(and control file point)
		readSize = m_file.read(pInputBuf, size, offset, whence);
	}
	if (readSize < 0) {
		printf("error: File::read() failed: 0x%08X\n", readSize);
	}
	return readSize;
}

int FileOutputStream::output(uint32_t size)
{
	int ret = 0;

	const void *p = m_buffer.lockReadingRoom(size);
	if (p) {
		ret = m_file.write(p, m_buffer.readingSize());
		if (ret < 0) {
			printf("error: File::write() failed: 0x%08X\n", ret);
			goto term;
		}
	}

term:
	if (p) {
		if (ret < 0) {
			m_buffer.unlockReadingRoom(0);
		} else {
			m_buffer.unlockReadingRoom(ret);
			ret = 0;
		}
	}
	return ret;
}

int AudioOutputStream::output(uint32_t size)
{
	int ret = 0;

	size = size ? size : m_audioOutput.size();

	if (size != m_audioOutput.size()) {
		return -1;
	}

	const void *p = m_buffer.lockReadingRoom(m_audioOutput.size());
	if (p) {
		ret = m_audioOutput.output(p);
		if (ret < 0) {
			printf("error: AudioOutput::output() failed: 0x%08X\n", ret);
			goto term;
		}
	}

term:
	if (p) {
		if (ret < 0) {
			m_buffer.unlockReadingRoom(0);
		} else if (m_hasOutput) {
			m_buffer.unlockReadingRoom(m_audioOutput.size(), m_audioOutput.size());
		} else {
			m_hasOutput = true;
		}
	} else {
		m_buffer.unlockReadingRoom(m_buffer.readingSize());
	}
	return ret;
}
