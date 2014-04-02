/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2013 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "ring_buffer.h"

using namespace Media;

RingBuffer::RingBuffer(uint32_t size, uint32_t sizeMargin) :
	m_p(0),
	m_size(size),
	m_sizeMargin(sizeMargin),
	m_mtx(0),
	m_cond(0),
	m_hasEnded(false),
	m_isRunning(true)
{
	int ret = 0;

	assert(sizeMargin < size);
	assert(sizeMargin <= size - sizeMargin);

	// allocate a buffer
	m_p = new uint8_t[size];
	if (!m_p) {
		printf("error: no more memory of %u byte\n", size);
		goto term;
	}
	// make the initial buffer list
	{
		BufferList *writingRoom = unusedWritingRoom();
		headRoom()->initialize(m_p, sizeMargin);
		writingRoom->initialize(headRoom()->tail(), size - sizeMargin);
		headRoom()->insert(writingRoom);
		tailRoom()->initialize(writingRoom->tail(), 0);
		writingRoom->insert(tailRoom());
	}

	// create a mutex for this instance
	ret = scePthreadMutexInit(&m_mtx, 0, "RingBuffer");
	if (ret < 0) {
		printf("error: scePthreadMutexInit() failed: 0x%08X\n", ret);
		goto term;
	}
	// create a condition variable
	ret = scePthreadCondInit(&m_cond, 0, "RingBuffer");
	if (ret < 0) {
		printf("error: scePthreadCondInit() failed: 0x%08X\n", ret);
		goto term;
	}

term:
	return;
}

RingBuffer::~RingBuffer(void)
{
	int tmp;

	// delete a condition variable
	tmp = scePthreadCondDestroy(&m_cond);
	if (tmp < 0) {
		printf("error: scePthreadCondDestroy() failed: 0x%08X\n", tmp);
	}
	// delete a mutex for this instance
	tmp = scePthreadMutexDestroy(&m_mtx);
	if (tmp < 0) {
		printf("error: scePthreadMutexDestroy() failed: 0x%08X\n", tmp);
	}

	// free a buffer
	if (m_p) {
		delete [] m_p;
	}
}

uint32_t RingBuffer::numWritingRooms(void) const
{
	uint32_t numWritingRooms = 0;
	if (m_writingRoom[0].size()) {
		numWritingRooms++;
	}
	if (m_writingRoom[1].size()) {
		numWritingRooms++;
	}
	return numWritingRooms;
}

uint32_t RingBuffer::numReadingRooms(void) const
{
	uint32_t numReadingRooms = 0;
	if (m_readingRoom[0].size()) {
		numReadingRooms++;
	}
	if (m_readingRoom[1].size()) {
		numReadingRooms++;
	}
	return numReadingRooms;
}

BufferList *RingBuffer::writingRoom0(void)
{
	BufferList *p = 0;
	assert(m_writingRoom[0].head() != m_writingRoom[1].head());
	if (m_writingRoom[0].head() < m_writingRoom[1].head()) {
		p = &m_writingRoom[1];
	}
	if (m_writingRoom[1].head() < m_writingRoom[0].head()) {
		p = &m_writingRoom[0];
	}
	return p;
}

BufferList *RingBuffer::writingRoom1(void)
{
	BufferList *p = 0;
	assert(m_writingRoom[0].head() != m_writingRoom[1].head());
	if (m_writingRoom[0].head() < m_writingRoom[1].head()) {
		p = &m_writingRoom[0];
	}
	if (m_writingRoom[1].head() < m_writingRoom[0].head()) {
		p = &m_writingRoom[1];
	}
	return p;
}

BufferList *RingBuffer::unusedWritingRoom(void)
{
	BufferList *p = 0;
	if (!m_writingRoom[0].previous() && !m_writingRoom[0].next()) {
		p = &m_writingRoom[0];
	}
	if (!m_writingRoom[1].previous() && !m_writingRoom[1].next()) {
		p = &m_writingRoom[1];
	}
	return p;
}

BufferList *RingBuffer::readingRoom0(void)
{
	BufferList *p = 0;
	assert(m_readingRoom[0].head() != m_readingRoom[1].head());
	if (m_readingRoom[0].head() < m_readingRoom[1].head()) {
		p = &m_readingRoom[1];
	}
	if (m_readingRoom[1].head() < m_readingRoom[0].head()) {
		p = &m_readingRoom[0];
	}
	return p;
}

BufferList *RingBuffer::readingRoom1(void)
{
	BufferList *p = 0;
	assert(m_readingRoom[0].head() != m_readingRoom[1].head());
	if (m_readingRoom[0].head() < m_readingRoom[1].head()) {
		p = &m_readingRoom[0];
	}
	if (m_readingRoom[1].head() < m_readingRoom[0].head()) {
		p = &m_readingRoom[1];
	}
	return p;
}

BufferList *RingBuffer::availableReadingRoom(void)
{
	BufferList *p = 0;
	if (readingSize() < readingRoom0()->size()) {
		p = readingRoom0();
	} else {
		p = readingRoom1();
	}
	return p;
}

BufferList *RingBuffer::unusedReadingRoom(void)
{
	BufferList *p = 0;
	if (!m_readingRoom[0].previous() && !m_readingRoom[0].next()) {
		p = &m_readingRoom[0];
	}
	if (!m_readingRoom[1].previous() && !m_readingRoom[1].next()) {
		p = &m_readingRoom[1];
	}
	return p;
}

void RingBuffer::reserveWritingRoom(uint32_t size)
{
	const uint32_t availableSize = writingRoom0()->availableSize();
	if (availableSize < size) {
		writingRoom0()->reserve(availableSize);
		writingRoom1()->reserve(size - availableSize);
	} else {
		writingRoom0()->reserve(size);
	}
}

void RingBuffer::releaseWritingRoom(uint32_t size)
{
	const uint32_t reservedSize = writingRoom0()->reservedSize();
	if (reservedSize < size) {
		writingRoom0()->release(reservedSize);
		writingRoom1()->release(size - reservedSize);
	} else {
		writingRoom0()->release(size);
	}
}

void RingBuffer::reserveReadingRoom(uint32_t size)
{
	const uint32_t availableSize = readingRoom0()->availableSize();
	if (availableSize < size) {
		readingRoom0()->reserve(availableSize);
		readingRoom1()->reserve(size - availableSize);
	} else {
		readingRoom0()->reserve(size);
	}
}

void RingBuffer::releaseReadingRoom(uint32_t size)
{
	const uint32_t reservedSize = readingRoom0()->reservedSize();
	if (reservedSize < size) {
		readingRoom0()->release(reservedSize);
		readingRoom1()->release(size - reservedSize);
	} else {
		readingRoom0()->release(size);
	}
}

int RingBuffer::circulateWritingRoom(uint32_t size)
{
	int ret = -1;

	while (m_isRunning) {
		scePthreadMutexLock(&m_mtx);
		assert(isTailOfBuffer(writingRoom0()));
		if (marginSize()) {
			// margin size (m_sizeMargin) was specified when initializing
			if (writingRoom1()->size() + readingRoom0()->size() < size) {
				printf("error: specified buffer size (m_size) is too small when initializing\n");
				scePthreadMutexUnlock(&m_mtx);
				assert(0);
				break;
			}
			// circulate the writing room only when no writing room is locked
			if (!writingSize() && isHeadOfBuffer(writingRoom1()) && size <= writingRoom1()->size()) {
				assert(headRoom()->size() == marginSize());
				// remove the writing room
				writingRoom0()->reduceTail(writingRoom0()->size());
				writingRoom0()->remove();
				assert(isTailOfBuffer(readingRoom0()));
				scePthreadMutexUnlock(&m_mtx);
				ret = 0;
				break;
			} else {
				scePthreadCondWait(&m_cond, &m_mtx);
			}
		} else {
			const uint32_t readingRoomSize = readingRoom0()->size();
			// circulate both the writing room and the reading room
			if (!writingSize() && !readingSize() && isHeadOfBuffer(writingRoom1()) && readingRoomSize <= writingRoom1()->size()) {
				// remove the writing room
				writingRoom0()->reduceTail(writingRoom0()->size());
				writingRoom0()->remove();
				assert(isTailOfBuffer(readingRoom0()));
				// insert a new reading room
				headRoom()->insert(unusedReadingRoom());
				// copy the reading room
				if (readingRoomSize) {
					writingRoom0()->reduceHead(readingRoomSize);
					memcpy(readingRoom1()->head(), readingRoom0()->head(), readingRoomSize);
					readingRoom0()->reduceTail(readingRoomSize);
				}
				// remove the reading room
				readingRoom0()->remove();
				tailRoom()->reduceHead(tailRoom()->size());
				assert(isTailOfBuffer(writingRoom0()));
				scePthreadMutexUnlock(&m_mtx);
				ret = 0;
				break;
			} else {
				scePthreadCondWait(&m_cond, &m_mtx);
			}
		}
	}

	return ret;
}

int RingBuffer::circulateReadingRoom(uint32_t size)
{
	int ret = -1;

	while (m_isRunning) {
		scePthreadMutexLock(&m_mtx);
		if (marginSize()) {
			assert(isTailOfBuffer(readingRoom0()));
			assert(readingRoom0() == availableReadingRoom());
			// margin size (m_sizeMargin) was specified when initializing
			if (marginSize() < readingRoom0()->availableSize()) {
				printf("error: specified margin size (m_sizeMargin) is too small when initializing\n");
				scePthreadMutexUnlock(&m_mtx);
				assert(0);
				break;
			}
			assert(isHeadOfBuffer(readingRoom1()));
			assert(headRoom()->size() == marginSize());
			// circulate the reading room even if any reading room is locked
			// copy the reading room
			if (readingRoom0()->availableSize()) {
				headRoom()->reduceTail(readingRoom0()->availableSize());
				memcpy(readingRoom1()->head(), readingRoom0()->head() + readingRoom0()->reservedSize(), readingRoom0()->availableSize());
				readingRoom0()->reduceTail(readingRoom0()->availableSize());
			}
			// remove the reading room
			if (!readingRoom0()->size()) {
				readingRoom0()->remove();
				tailRoom()->reduceHead(tailRoom()->size());
				assert(isTailOfBuffer(writingRoom0()));
			}
			scePthreadMutexUnlock(&m_mtx);
			ret = 0;
			break;
		} else {
			assert(isTailOfBuffer(writingRoom0()));
			scePthreadMutexUnlock(&m_mtx);
			ret = 0;
			break;
		}
	}

	return ret;
}

uint8_t *RingBuffer::lockWritingRoom(uint32_t size)
{
	uint8_t *p = 0;

	while (m_isRunning) {
		scePthreadMutexLock(&m_mtx);
		if (writingSize()) {
			printf("error: writing room can not be locked while previous writing room is locked\n");
			scePthreadMutexUnlock(&m_mtx);
			assert(0);
			break;
		}
		// if size is zero, use it as all of available room
		size = size ? size : writingRoomSize();
		// check the arguments
		if (ringSize() < size) {
			printf("error: specified buffer size (m_size) is too small when initializing\n");
			scePthreadMutexUnlock(&m_mtx);
			assert(0);
			break;
		}
		// lock a writing room
		if (size) {
			if (hasEnded()) {
				// it is impossible to lock any more writing room when the buffer has ended
				scePthreadMutexUnlock(&m_mtx);
				break;
			}
			if (size <= writingRoomSize()) {
				// when the buffer has enough space for a writing room
				if (tailRoom()->tail() < writingRoom0()->head() + size) {
					scePthreadMutexUnlock(&m_mtx);
					// circulate the writing room when it is divided into two parts
					int ret = circulateWritingRoom(size);
					if (ret < 0) {
						break;
					}
					scePthreadMutexLock(&m_mtx);
				}
				// lock a writing room
				p = writingRoom0()->head();
				reserveWritingRoom(size);
				scePthreadMutexUnlock(&m_mtx);
				break;
			} else {
				scePthreadCondWait(&m_cond, &m_mtx);
			}
		} else {
			if (hasEnded()) {
				// it is impossible to lock any more writing room when the buffer has ended
				scePthreadMutexUnlock(&m_mtx);
				break;
			}
			scePthreadCondWait(&m_cond, &m_mtx);
		}
	}

	return p;
}

int RingBuffer::unlockWritingRoom(uint32_t sizeWritten)
{
	int ret = 0;

	scePthreadMutexLock(&m_mtx);

	const uint32_t sizeUnlocked = writingSize();

	// check the arguments
	assert(sizeWritten <= writingSize());
	assert(writingRoom0()->head() + sizeWritten <= tailRoom()->tail());
	// insert a new reading room
	if (!isReadingRoom(writingRoom0()->previous())) {
		assert(isHeadOfBuffer(writingRoom0()));
		headRoom()->insert(unusedReadingRoom());
	}
	// unlock the writing room
	assert(isReadingRoom(writingRoom0()->previous()));
	writingRoom0()->reduceHead(sizeWritten);
	releaseWritingRoom(sizeUnlocked);
	// unlock the writing room at tail
	if (isTailOfBuffer(writingRoom0()) && !writingRoom0()->size() && marginSize()) {
		writingRoom0()->remove();
		isTailOfBuffer(readingRoom0());
	}
	scePthreadCondSignal(&m_cond);

	scePthreadMutexUnlock(&m_mtx);

	return ret;
}

const uint8_t *RingBuffer::lockReadingRoom(uint32_t size)
{
	const uint8_t *p = 0;

	while (m_isRunning) {
		scePthreadMutexLock(&m_mtx);
		// if size is zero, use it as all of available room
		size = size ? size : availableReadingRoomSize();
		// check the arguments
		if (ringSize() < size) {
			printf("error: specified buffer size (m_size) is too small when initializing\n");
			scePthreadMutexUnlock(&m_mtx);
			assert(0);
			break;
		}
		// lock a reading room
		if (size) {
			if (hasEnded() && !writingSize()) {
				if (!availableReadingRoomSize()) {
					// it is impossible to lock any more reading room when the buffer has ended and has no reading room
					scePthreadMutexUnlock(&m_mtx);
					break;
				}
				if (availableReadingRoomSize() < size) {
					bool canLock = false;
					// when the buffer does not have enough space for a reading room
					if (isTailOfBuffer(readingRoom0())) {
						if (readingRoom0() == availableReadingRoom()) {
							if (tailRoom()->tail() < readingRoom0()->head() + readingRoom0()->reservedSize() + size + tailRoom()->size()) {
								scePthreadMutexUnlock(&m_mtx);
								// circulate the reading room when it is divided into two parts
								int ret = circulateReadingRoom(size);
								if (ret < 0) {
									break;
								}
								scePthreadMutexLock(&m_mtx);
							}
							canLock = true;
						} else {
							if (availableReadingRoom()->head() + availableReadingRoom()->reservedSize() + size <= readingRoom0()->head()) {
								canLock = true;
							}
						}
					} else {
						if (tailRoom()->tail() < readingRoom0()->head() + readingRoom0()->reservedSize() + size) {
							scePthreadMutexUnlock(&m_mtx);
							// circulate the writing room when it is divided into two parts
							int ret = circulateWritingRoom(0);
							if (ret < 0) {
								break;
							}
							// circulate the reading room when it is divided into two parts
							ret = circulateReadingRoom(size);
							if (ret < 0) {
								break;
							}
							scePthreadMutexLock(&m_mtx);
						}
						canLock = true;
					}
					if (canLock) {
						// set zero over available reading room
						memset(availableReadingRoom()->tail(), 0, size - availableReadingRoom()->availableSize());
						// lock a reading room over available reading room
						p = availableReadingRoom()->head() + availableReadingRoom()->reservedSize();
						reserveReadingRoom(size);
						scePthreadMutexUnlock(&m_mtx);
						break;
					}
				}
			}
			if (size <= availableReadingRoomSize()) {
				// when the buffer has enough space for a reading room
				if (isTailOfBuffer(readingRoom0())) {
					if (readingRoom0() == availableReadingRoom()) {
						if (tailRoom()->tail() < readingRoom0()->head() + readingRoom0()->reservedSize() + size + tailRoom()->size()) {
							scePthreadMutexUnlock(&m_mtx);
							// circulate the reading room when it is divided into two parts
							int ret = circulateReadingRoom(size);
							if (ret < 0) {
								break;
							}
							scePthreadMutexLock(&m_mtx);
						}
					}
				}
				// lock a reading room
				p = availableReadingRoom()->head() + availableReadingRoom()->reservedSize();
				reserveReadingRoom(size);
				scePthreadMutexUnlock(&m_mtx);
				break;
			} else {
				scePthreadCondWait(&m_cond, &m_mtx);
			}
		} else {
			if (hasEnded()) {
				// it is impossible to lock any more reading room when the buffer has ended
				scePthreadMutexUnlock(&m_mtx);
				break;
			}
			scePthreadCondWait(&m_cond, &m_mtx);
		}
	}

	return p;
}

int RingBuffer::unlockReadingRoom(uint32_t sizeRead, uint32_t sizeUnlocked)
{
	int ret = 0;

	scePthreadMutexLock(&m_mtx);

	// check the arguments
	if (sizeUnlocked) {
		if (sizeUnlocked != sizeRead) {
			printf("error: sizeUnlocked is not same as sizeRead\n");
			assert(0);
		}
	} else {
		sizeUnlocked = readingSize();
	}
	assert(sizeRead <= readingSize());
	sizeRead = std::min(sizeRead, readingRoomSize());
	assert(readingRoom0()->head() + sizeRead <= tailRoom()->tail());
	// unlock the reading room at head
	if (isHeadOfBuffer(readingRoom0()) && headRoom()->size() < marginSize()) {
		const uint32_t size = std::min(sizeRead, marginSize() - headRoom()->size());
		readingRoom0()->reduceHead(size);
		sizeRead -= size;
	}
	if (sizeRead) {
		// insert a new writing room
		if (!isWritingRoom(readingRoom0()->previous())) {
			assert(isHeadOfBuffer(readingRoom0()));
			headRoom()->insert(unusedWritingRoom());
		}
		// unlock the reading room
		assert(isWritingRoom(readingRoom0()->previous()));
		readingRoom0()->reduceHead(sizeRead);
	}
	releaseReadingRoom(sizeUnlocked);
	// unlock the reading room at tail
	if (isTailOfBuffer(readingRoom0()) && !readingRoom0()->size()) {
		readingRoom0()->remove();
		tailRoom()->reduceHead(tailRoom()->size());
	}
	scePthreadCondSignal(&m_cond);

	scePthreadMutexUnlock(&m_mtx);

	return ret;
}

int RingBuffer::end(void)
{
	scePthreadMutexLock(&m_mtx);
	m_hasEnded = true;
	scePthreadCondSignal(&m_cond);
	scePthreadMutexUnlock(&m_mtx);
	return 0;
}
