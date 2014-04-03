/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2013 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#ifndef _RING_BUFFER_H
#define _RING_BUFFER_H

#include <stdint.h>
#include <assert.h>
#include <kernel.h>

namespace Media {

class Buffer
{
public:
	Buffer(void) : m_p(0), m_size(0) {}
	virtual ~Buffer(void) {}
	uint8_t *head(void) const { return m_p; }
	uint8_t *tail(void) const { return m_p + m_size; }
	uint32_t size(void) const { return m_size; }
protected:
	void initialize(uint8_t *p, uint32_t size)
	{
		m_p = p;
		m_size = size;
	}
	void extendHead(uint32_t size)
	{
		m_p -= size;
		m_size += size;
	}
	void extendTail(uint32_t size)
	{
		m_size += size;
	}
	void reduceHead(uint32_t size)
	{
		assert(size <= m_size);
		m_p += size;
		m_size -= size;
	}
	void reduceTail(uint32_t size)
	{
		assert(size <= m_size);
		m_size -= size;
	}
private:
	uint8_t *m_p;
	uint32_t m_size;
};

class BufferList : public Buffer
{
public:
	BufferList(void) :
		m_pPrevious(0),
		m_pNext(0),
		m_sizeReserved(0)
	{}
	~BufferList(void) {}
	void initialize(uint8_t *p, uint32_t size)
	{
		assert(!m_pPrevious);
		assert(!m_pNext);
		assert(!m_sizeReserved);
		Buffer::initialize(p, size);
	}
	BufferList *previous(void) { return m_pPrevious; }
	BufferList *next(void) { return m_pNext; }
	uint32_t reservedSize(void) const { return m_sizeReserved; }
	uint32_t availableSize(void) const { return (reservedSize() < size()) ? size() - reservedSize() : 0; }
	void insert(BufferList *p)
	{
		assert(p);
		// initialize the specified one
		p->initialize(tail(), p->size());
		// insert the specified one
		if (next()) {
			assert(p->tail() == next()->head());
			next()->previous(p);
		}
		p->previous(this);
		p->next(next());
		next(p);
	}
	void remove(void)
	{
		assert(!size());
		assert(!reservedSize());
		assert(previous()->tail() == next()->head());
		// remove the self
		previous()->next(next());
		next()->previous(previous());
		previous(0);
		next(0);
		initialize(0, 0);
	}
	void reduceHead(uint32_t size)
	{
		assert(previous());
		previous()->Buffer::extendTail(size);
		Buffer::reduceHead(size);
		assert(previous()->tail() == head());
	}
	void reduceTail(uint32_t size)
	{
		assert(next());
		Buffer::reduceTail(size);
		next()->Buffer::extendHead(size);
		assert(tail() == next()->head());
	}
	void reserve(uint32_t size)
	{
		m_sizeReserved += size;
	}
	void release(uint32_t size)
	{
		assert(size <= reservedSize());
		m_sizeReserved -= size;
	}
private:
	void previous(BufferList *p) { m_pPrevious = p; }
	void next(BufferList *p) { m_pNext = p; }
	BufferList *m_pPrevious;
	BufferList *m_pNext;
	uint32_t m_sizeReserved;
};

class RingBuffer
{
public:
	RingBuffer(uint32_t size, uint32_t sizeMargin);
	~RingBuffer(void);
	uint32_t size(void) const { return m_size; }
	bool isEmpty(void) const { return (readingRoomSize() == 0); }
	uint8_t *lockWritingRoom(uint32_t size);
	int unlockWritingRoom(uint32_t sizeWritten);
	uint32_t writingSize(void) const { return m_writingRoom[0].reservedSize() + m_writingRoom[1].reservedSize(); }
	const uint8_t *lockReadingRoom(uint32_t size);
	int unlockReadingRoom(uint32_t sizeRead, uint32_t sizeUnlocked = 0);
	uint32_t readingSize(void) const { return m_readingRoom[0].reservedSize() + m_readingRoom[1].reservedSize(); }
	int end(void);
	void stopRunning() { m_isRunning = false; }
private:
	uint32_t ringSize(void) const { return m_size - m_sizeMargin; }
	uint32_t marginSize(void) const { return m_sizeMargin; }
	uint32_t writingRoomSize(void) const { return m_writingRoom[0].size() + m_writingRoom[1].size(); }
	uint32_t readingRoomSize(void) const { return m_readingRoom[0].size() + m_readingRoom[1].size(); }
	uint32_t availableReadingRoomSize(void) const { return m_readingRoom[0].availableSize() + m_readingRoom[1].availableSize(); }
	uint32_t numWritingRooms(void) const;
	uint32_t numReadingRooms(void) const;
	BufferList *headRoom(void) { return &m_head; }
	BufferList *tailRoom(void) { return &m_tail; }
	BufferList *writingRoom0(void);
	BufferList *writingRoom1(void);
	BufferList *unusedWritingRoom(void);
	BufferList *readingRoom0(void);
	BufferList *readingRoom1(void);
	BufferList *availableReadingRoom(void);
	BufferList *unusedReadingRoom(void);
	bool isHeadOfBuffer(BufferList *p) { return (p == m_head.next()); }
	bool isTailOfBuffer(BufferList *p)
	{
		if (isReadingRoom(m_tail.previous())) {
			assert(numWritingRooms() <= 1);
		} else if (isWritingRoom(m_tail.previous())) {
			assert(numReadingRooms() <= 1);
			assert(!m_tail.size());
		}
		return (p == m_tail.previous());
	}
	bool isWritingRoom(BufferList *p) const { return (p == &m_writingRoom[0] || p == &m_writingRoom[1]); }
	bool isReadingRoom(BufferList *p) const { return (p == &m_readingRoom[0] || p == &m_readingRoom[1]); }
	void reserveWritingRoom(uint32_t size);
	void releaseWritingRoom(uint32_t size);
	void reserveReadingRoom(uint32_t size);
	void releaseReadingRoom(uint32_t size);
	int circulateWritingRoom(uint32_t size);
	int circulateReadingRoom(uint32_t size);
	bool hasEnded(void) const { return m_hasEnded; }
	uint8_t *m_p;
	const uint32_t m_size;
	const uint32_t m_sizeMargin;
	BufferList m_head;
	BufferList m_tail;
	BufferList m_writingRoom[2];
	BufferList m_readingRoom[2];
	ScePthreadMutex m_mtx;
	ScePthreadCond m_cond;
	bool m_hasEnded;
	bool m_isRunning;
};

} // namespace Media

#endif /* _RING_BUFFER_H */
