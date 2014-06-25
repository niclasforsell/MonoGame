#pragma once

#include <gnm/gpumem.h>
#include <stddef.h>

namespace mem {

	//===========================================
	// Memory for fast CPU read/write
	//===========================================

	void* alloc(size_t size, uint32_t align = 4);

	template <typename T = void>
	T* alloc(size_t size = sizeof(T), uint32_t align = 4)
	{
		return (T*)alloc(size, align);
	}

	template <typename T = void>
	T* alloc_array(int count, uint32_t align = 4)
	{
		return (T*)alloc(sizeof(T) * count, align);
	}

	void free(void* ptr);

	//===========================================
	// Memory shared between CPU and GPU
	//===========================================

	void* allocShared(size_t size, uint32_t align = 4);
	void* allocShared(sce::Gnm::SizeAlign sizeAlign);

	template <typename T = void>
	T* allocShared(size_t size = sizeof(T), uint32_t align = 4)
	{
		return (T*)allocShared(size, align);
	}

	void freeShared(void* ptr);
}
