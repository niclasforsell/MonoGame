#pragma once

#include "predecls.h"
#include <memory>
#include <scebase.h>
#include <playgo.h>

namespace System {

enum PlayGoResult
{
	Ok = SCE_OK,

	ErrorUnknown = SCE_PLAYGO_ERROR_UNKNOWN,
	ErrorFatal = SCE_PLAYGO_ERROR_FATAL,
	ErrorNoMemory = SCE_PLAYGO_ERROR_NO_MEMORY,
	ErrorInvalidArgument = SCE_PLAYGO_ERROR_INVALID_ARGUMENT,
	ErrorNotInitialized = SCE_PLAYGO_ERROR_NOT_INITIALIZED,
	ErrorAlreadyInitialized = SCE_PLAYGO_ERROR_ALREADY_INITIALIZED,
	ErrorAlreadyStarted = SCE_PLAYGO_ERROR_ALREADY_STARTED,
	ErrorNotStarted = SCE_PLAYGO_ERROR_NOT_STARTED,
	ErrorBadHandle = SCE_PLAYGO_ERROR_BAD_HANDLE,
	ErrorBadPointer = SCE_PLAYGO_ERROR_BAD_POINTER,
	ErrorBadSize = SCE_PLAYGO_ERROR_BAD_SIZE,
	ErrorBadChunkId = SCE_PLAYGO_ERROR_BAD_CHUNK_ID,
	ErrorBadSpeed = SCE_PLAYGO_ERROR_BAD_SPEED,
	ErrorNotSupportPlaygo = SCE_PLAYGO_ERROR_NOT_SUPPORT_PLAYGO,
	ErrorEPerm = SCE_PLAYGO_ERROR_EPERM,
	ErrorBadLocus = SCE_PLAYGO_ERROR_BAD_LOCUS
};

enum PlayGoInstallSpeed : int32_t
{
	Suspended = 0,
	Trickle = 1,
	Full = 2
};

/* TODO: Address this after 64-bit fix
enum PlayGoLanguageMask : uint64_t
{
	All = SCE_PLAYGO_LANGUAGE_MASK_ALL
};
*/

enum PlayGoLocus : int8_t
{
	NotDownloaded = SCE_PLAYGO_LOCUS_NOT_DOWNLOADED,
	LocalSlow = SCE_PLAYGO_LOCUS_LOCAL_SLOW,
	Fast = SCE_PLAYGO_LOCUS_LOCAL_FAST
};

class CS_API ChunkList
{
public:
	CS_IGNORE static const int MaxItems = SCE_PLAYGO_CHUNK_INDEX_MAX;
	CS_IGNORE uint32_t _count;
	CS_IGNORE ScePlayGoChunkId _items[MaxItems];

	ChunkList()
	{
		_count = 0;
		memset(_items, 0, sizeof(ScePlayGoChunkId) * MaxItems);
	}

protected: // Hide from end users
	bool TryAdd(ScePlayGoChunkId value)
	{
		if (_count >= MaxItems)
			return false;

		_items[_count] = value;
		_count++;

		return true;
	}
};

class CS_API LociResult
{
public:
	CS_IGNORE static const int MaxItems = SCE_PLAYGO_CHUNK_INDEX_MAX;
	CS_IGNORE uint32_t _count;
	CS_IGNORE int32_t _result;
	CS_IGNORE ScePlayGoLocus _items[MaxItems];

	LociResult()
	{
		_count = 0;
		memset(_items, 0, sizeof(ScePlayGoLocus) * MaxItems);
	}

	uint32_t GetCount() const
	{
		return _count;
	}

	int32_t GetResult() const
	{
		return _result;
	}

protected: // Hide from end users
	bool TryGet(int index, CS_OUT ScePlayGoLocus* outLocus)
	{
		if (index >= MaxItems)
			return false;

		*outLocus = _items[index];

		return true;
	}
};

class CS_API ToDoList
{
public:
	CS_IGNORE static const int MaxItems = SCE_PLAYGO_CHUNK_INDEX_MAX;
	CS_IGNORE uint32_t _count;
	CS_IGNORE ScePlayGoToDo _items[MaxItems];

	ToDoList()
	{
		_count = 0;
		memset(_items, 0, sizeof(ScePlayGoToDo) * MaxItems);
	}

	uint32_t GetCount() const
	{
		return _count;
	}

protected: // Hide from end users

	bool TryAdd(ScePlayGoChunkId chunkId, ScePlayGoLocus locus)
	{
		if (_count >= MaxItems)
			return false;

		_items[_count].chunkId = chunkId;
		_items[_count].locus = locus;
		_count++;

		return true;
	}

	bool TryGet(int index, CS_OUT ScePlayGoChunkId* chunkId, CS_OUT ScePlayGoLocus* outLocus)
	{
		if (index >= MaxItems)
			return false;

		*chunkId = _items[index].chunkId;
		*outLocus = _items[index].locus;

		return true;
	}
};

class CS_API ProgressResult
{
public:
	CS_IGNORE static const int MaxItems = SCE_PLAYGO_CHUNK_INDEX_MAX;
	CS_IGNORE uint32_t _count;
    CS_IGNORE int32_t _result;
	CS_IGNORE ScePlayGoProgress _items[MaxItems];

	ProgressResult()
	{
		_count = 0;
		memset(_items, 0, sizeof(ScePlayGoProgress) * MaxItems);
	}

	uint32_t GetCount() const
	{
		return _count;
	}

	int32_t GetError() const
	{
		return _result;
	}

protected: // Hide from end users
	bool TryGet(int index, CS_OUT uint64_t* progressSize, CS_OUT uint64_t* totalSize)
	{
		if (index >= _count)
			return false;

		*progressSize = _items[index].progressSize;
		*totalSize = _items[index].totalSize;

		return true;
	}
};

class CS_API PlayGo
{
	void* _heap;
	ScePlayGoHandle _handle;

public:
	PlayGo(uint32_t heapSize);
	PlayGo(const PlayGo&) = delete;
	~PlayGo();

	static uint32_t GetDefaultBufferSize() { return SCE_PLAYGO_HEAP_SIZE; }
	static ScePlayGoLanguageMask ConvertLanguage(int32_t systemLang);

	time_t GetEta(const ChunkList* chunks);

	PlayGoInstallSpeed GetInstallSpeed();
	void SetInstallSpeed(PlayGoInstallSpeed speed);

	ScePlayGoLanguageMask GetLanguageMask();
	void SetLanguageMask(ScePlayGoLanguageMask languageMask);

	LociResult* GetLocus(const ChunkList* chunks);

	ProgressResult* GetProgress(const ChunkList* chunks);

	ToDoList* GetToDoList();
	void SetToDoList(ToDoList* toDoList);

	PlayGoResult Prefetch(const ChunkList* chunks, ScePlayGoLocus minimumLocus);
};

} // namespace System
