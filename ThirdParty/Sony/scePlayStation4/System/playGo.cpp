#include "playGo.h"
#include "../allocator.h"

#include <assert.h>
#include <libsysmodule.h>
#include <playgo.h>

using namespace System;

PlayGo::PlayGo(uint32_t heapSize)
	: _heap(nullptr),
	_handle(-1)
{
	if (sceSysmoduleIsLoaded(SCE_SYSMODULE_PLAYGO) == SCE_SYSMODULE_ERROR_UNLOADED)
	{
		auto ret = sceSysmoduleLoadModule(SCE_SYSMODULE_PLAYGO);
		assert(ret == SCE_OK);
	}

	assert(heapSize > 0);
	_heap = Allocator::Get()->allocate(heapSize, 512UL);

	ScePlayGoInitParams params;
	memset(&params, 0, sizeof(ScePlayGoInitParams));
	params.bufSize = heapSize;
	params.bufAddr = _heap;
	params.reserved = NULL;

	auto ret = scePlayGoInitialize(&params);
	if (ret != SCE_OK)
		printf("ERROR: Couldn't initialize PlayGo: 0x%08X\n", ret);
	assert(ret == SCE_OK);

	ret = scePlayGoOpen(&_handle, NULL);
	if (ret != SCE_OK)
		printf("ERROR: Couldn't open PlayGo: 0x%08X\n", ret);
	assert(ret == SCE_OK);
}

PlayGo::~PlayGo()
{
	auto ret = scePlayGoClose(_handle);
	assert(ret == SCE_OK);

	auto termRet = scePlayGoTerminate();
	assert(termRet == SCE_OK);

	Allocator::Get()->release(_heap);

	if (sceSysmoduleIsLoaded(SCE_SYSMODULE_PLAYGO) == SCE_SYSMODULE_LOADED)
	{
		auto ret = sceSysmoduleUnloadModule(SCE_SYSMODULE_PLAYGO);
		assert (ret == SCE_OK);
	}
}

ScePlayGoLanguageMask PlayGo::ConvertLanguage(int32_t systemLang)
{
	return scePlayGoConvertLanguage(systemLang);
}

time_t PlayGo::GetEta(const ChunkList* chunks)
{
	ScePlayGoEta result;
	auto ret = scePlayGoGetEta(_handle, chunks->_items, chunks->_count, &result);
	assert(ret == SCE_OK);
	return result;
}

PlayGoInstallSpeed PlayGo::GetInstallSpeed()
{
	ScePlayGoInstallSpeed result;
	auto ret = scePlayGoGetInstallSpeed(_handle, &result);
	assert(ret == SCE_OK);
	return (PlayGoInstallSpeed)result;
}

void PlayGo::SetInstallSpeed(PlayGoInstallSpeed speed)
{
	auto ret = scePlayGoSetInstallSpeed(_handle, (ScePlayGoInstallSpeed)speed);
	assert(ret == SCE_OK);
}

ScePlayGoLanguageMask PlayGo::GetLanguageMask()
{
	ScePlayGoLanguageMask result;
	auto ret = scePlayGoGetLanguageMask(_handle, &result);
	assert(ret == SCE_OK);
	return result;
}

void PlayGo::SetLanguageMask(ScePlayGoLanguageMask languageMask)
{
	auto ret = scePlayGoSetLanguageMask(_handle, languageMask);
	assert(ret == SCE_OK);
}

LociResult* PlayGo::GetLocus(const ChunkList* chunks)
{
	auto result = new LociResult();

	result->_result = scePlayGoGetLocus(_handle, chunks->_items, chunks->_count, result->_items);
	if (result->_result == SCE_OK)
		result->_count = chunks->_count;

	return result;
}

ProgressResult* PlayGo::GetProgress(const ChunkList* chunks)
{
	auto result = new ProgressResult();

	result->_result = scePlayGoGetProgress(_handle, chunks->_items, chunks->_count, result->_items);
	if (result->_result == SCE_OK)
		result->_count = chunks->_count;

	return result;
}

ToDoList* PlayGo::GetToDoList()
{
	auto result = new ToDoList();

	auto ret = scePlayGoGetToDoList(_handle, result->_items, result->MaxItems, &result->_count);
	assert(ret == SCE_OK);

	return result;
}

void PlayGo::SetToDoList(ToDoList* toDoList)
{
	auto ret = scePlayGoSetToDoList(_handle, toDoList->_items, toDoList->_count);
	assert(ret == SCE_OK);
}

PlayGoResult PlayGo::Prefetch(const ChunkList* chunks, ScePlayGoLocus minimumLocus)
{
	auto ret = scePlayGoPrefetch(_handle, chunks->_items, chunks->_count, minimumLocus);
	return (PlayGoResult)ret;
}
