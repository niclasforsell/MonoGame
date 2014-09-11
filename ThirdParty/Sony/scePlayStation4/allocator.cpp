#include "allocator.h"

#include <sys/dmem.h>
#include <kernel.h>

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <string.h>

using namespace sce;

#define DISABLE_MEMORY_CLEAR 0
#define GNM_MEMORY_DEBUG_OVERFLOW_CHECK 0

namespace {
	const uint64_t kRegionTypeMask = 0xE000000000000000ULL;
	const uint64_t kRegionStartMask = 0x1FFFFFFFFFFFFFFFULL;

	const uint64_t kUnusedRegionType      = 0x0000000000000000ULL;
	const uint64_t kSystemRegionType      = 0x2000000000000000ULL;
	const uint64_t kSharedVideoRegionType = 0x4000000000000000ULL;
	const uint64_t kPrivateVideoRegionType= 0x8000000000000000ULL;

	const uint64_t kInitialOnionRegions = 4096;
	const uint64_t kInitialGarlicRegions = 3072;
	const uint64_t kRegionTableGrowthFactor = 2;

#ifndef NDEBUG
	const int kMaxHumanSizeBuffer = 16;
	char buffer[kMaxHumanSizeBuffer];
	const char* humanSize(double size)
	{
		const int kMaxSuffix = 4;
		const char* suffixes[kMaxSuffix] = { "B ", "KB", "MB", "GB" };
		auto mag = 0;
		while (size >= 1024.0 && ++mag < kMaxSuffix)
			size /= 1024.0;

		memset(buffer, 0, kMaxHumanSizeBuffer);
		snprintf(buffer, kMaxHumanSizeBuffer, "%3.2f %s", size, suffixes[mag]);

		return buffer;
	}
#endif
}

struct Region
{
	uint64_t m_typeAndStart;
	uint32_t m_size;
	uint32_t m_guard;
	Region *m_next;

#if GNM_MEMORY_STORE_ALLOC_FILE
	const char *m_file;
	uint32_t    m_line;
#endif // GNM_MEMORY_STORE_ALLOC_FILE
};

class RegionAllocator
{
	ScePthreadMutex m_lock;
	Region *m_freeRegions;
	Region *m_usedRegions;
	Region *m_allRegions;
	uint32_t m_numRegions;
	uint64_t m_memoryStart;
	uint64_t m_memoryEnd;

	uint64_t m_totalMemoryInUse;

private:

	Region* findUnusedRegion();
	Region* allocateRegion(uint32_t size, uint64_t type, uint32_t alignment);
	void	releaseRegion(uint64_t start, uint64_t type);
public:
	void	init(uint64_t memStart, uint32_t memSize, uint32_t numRegions);

	void*	allocate(uint32_t size, sce::Gnm::AlignmentType alignment);
	void*	allocate(sce::Gnm::SizeAlign sz) { return allocate(sz.m_size, sz.m_align); }
	void	release(void*);

	size_t	getMemoryUsage() { return m_totalMemoryInUse; }
};

class Allocator
{
public:

	static Allocator* Get();

	void* allocate(size_t size, size_t align = 4, SceKernelMemoryType type = SCE_KERNEL_WB_ONION);
	void* allocate(sce::Gnm::SizeAlign sizeAlign, SceKernelMemoryType type);

	void release(void *ptr);

private:

	Allocator();
	void initialize();

	static void mapMemory(	uint64_t *onion, uint32_t* onionSize,
		uint64_t *garlic, uint32_t* garlicSize);

	static Allocator _instance;

	bool _isInitialized;

	uint64_t m_onionMemAddr;
	uint64_t m_garlicMemAddr;
	uint32_t m_onionMemSize;
	uint32_t m_garlicMemSize;

	RegionAllocator m_onionAllocator;
	RegionAllocator m_garlicAllocator;
};

#define MUTEX_LOCK													\
{																\
	auto ret = scePthreadMutexLock(&m_lock);					\
	if (ret)													\
	printf("scePthreadMutexLock failed: 0x%08X\n", ret);	\
}

#define MUTEX_UNLOCK												\
{																\
	auto ret = scePthreadMutexUnlock(&m_lock);					\
	if (ret)													\
	printf("scePthreadMutexUnlock failed: 0x%08X\n", ret);	\
}

void Allocator::mapMemory(uint64_t* system, uint32_t* systemSize, uint64_t* gpuShared, uint32_t* gpuSharedSize)
{
	assert(system && systemSize && gpuShared && gpuSharedSize);

	// Setup the onion and garlic memory pools.
	const uint32_t	systemPoolSize	   = 1024 * 1024 * 2048;
	const uint32_t	sharedGpuPoolSize  = 1024 * 1024 * 1024;
	const uint32_t  systemAlignment		= 2 * 1024 * 1024;
	const uint32_t  shaderGpuAlignment	= 2 * 1024 * 1024;

	off_t systemPoolOffset;		// TODO: store these value to be able to free them later.
	off_t sharedGpuPoolOffset;

	void*	systemPoolPtr	  = NULL;
	void*	sharedGpuPoolPtr  = NULL;

	uint32_t result = SCE_GNM_OK;

	int retSys = sceKernelAllocateDirectMemory(0,
		SCE_KERNEL_MAIN_DMEM_SIZE,
		systemPoolSize,
		systemAlignment, // alignment
		SCE_KERNEL_WB_ONION,
		&systemPoolOffset);
	if ( !retSys )
	{
		retSys = sceKernelMapDirectMemory(&systemPoolPtr,
			systemPoolSize,
			SCE_KERNEL_PROT_CPU_READ|SCE_KERNEL_PROT_CPU_WRITE|SCE_KERNEL_PROT_GPU_ALL,
			0,						//flags
			systemPoolOffset,
			systemAlignment);
	}
	if ( retSys )
	{
		fprintf(stderr, "sce::Gnm::Initialize Error: System Memory Allocation Failed (%d)\n", retSys);
		result = SCE_GNM_ERROR_FAILURE;
	}


	//


	int retShr = sceKernelAllocateDirectMemory(0,
		SCE_KERNEL_MAIN_DMEM_SIZE,
		sharedGpuPoolSize,
		shaderGpuAlignment,	// alignment
		SCE_KERNEL_WC_GARLIC,
		&sharedGpuPoolOffset);

	if ( !retShr )
	{
		retShr = sceKernelMapDirectMemory(&sharedGpuPoolPtr,
			sharedGpuPoolSize,
			SCE_KERNEL_PROT_CPU_READ|SCE_KERNEL_PROT_CPU_WRITE|SCE_KERNEL_PROT_GPU_ALL,
			0,						//flags
			sharedGpuPoolOffset,
			shaderGpuAlignment);
	}
	if ( retShr )
	{
		fprintf(stderr, "sce::Gnm::Initialize Error: Shared Gpu Memory Allocation Failed (%d)\n", retShr);
		result = SCE_GNM_ERROR_FAILURE;
	}
	//

	if ( result == SCE_GNM_OK )
	{

		*gpuShared = (uint64_t)sharedGpuPoolPtr;
		*gpuSharedSize    = sharedGpuPoolSize;

		*system = (uint64_t)systemPoolPtr;
		*systemSize    = systemPoolSize;
	}

#ifndef DISABLE_MEMORY_CLEAR
	// clear all shared memory (including the memory we just used to clear private mem)
	// We clear to 0x7n because 0x7n7n7n7n is an obviously-invalid (Type-1) PM4 packet
	memset(system, 0x7d, systemSize);
	memset(gpuShared,  0x11, gpuSharedSize);
#endif // DISABLE_MEMORY_CLEAR

#ifndef NDEBUG
	fprintf(stderr,"Total Direct Memory         : %s\n", humanSize(SCE_KERNEL_MAIN_DMEM_SIZE));
	fprintf(stderr,"Allocator System/Onion Size : %s (0x%p - 0x%p)\n", humanSize(systemPoolSize), (uint64_t)systemPoolPtr, (uint64_t)systemPoolPtr + systemPoolSize);
	fprintf(stderr,"Allocator Video/Garlic Size : %s (0x%p - 0x%p)\n", humanSize(sharedGpuPoolSize), (uint64_t)sharedGpuPoolPtr, (uint64_t)sharedGpuPoolPtr + sharedGpuPoolSize);
#endif
}


void RegionAllocator::init(uint64_t memStart, uint32_t memSize, uint32_t numRegions)
{
	m_memoryStart = memStart;
	m_memoryEnd = memStart + memSize;
	m_numRegions = numRegions;
	m_freeRegions = NULL;
	m_usedRegions = NULL;
	m_totalMemoryInUse = 0;

	auto ret = scePthreadMutexInit(&m_lock, NULL, NULL);
	if (ret) 
		printf("scePthreadMutexInit failed: 0x%08X\n", ret);

	m_allRegions = (Region*)malloc(sizeof(Region) * m_numRegions * 10);
	memset(m_allRegions,0,sizeof(Region)*m_numRegions);
	if( memSize )
	{
		Region* r = findUnusedRegion();
		r->m_next = NULL;
		r->m_guard = 0;
		r->m_typeAndStart = m_memoryStart | kSystemRegionType;
		r->m_size = memSize;
		m_freeRegions = r;
		m_usedRegions = NULL;
	}
}

Region* RegionAllocator::findUnusedRegion()
{
	for (uint32_t i = 0; i < m_numRegions; ++i)
	{
		Region *r = &m_allRegions[i];
		if ((r->m_typeAndStart & kRegionTypeMask) == kUnusedRegionType)
			return r;
	}

	// Resize the region array to fit more handles
	auto newSize = m_numRegions * kRegionTableGrowthFactor;
	auto newRegions = (Region*)realloc(m_allRegions, sizeof(Region) * newSize);
	if (newRegions != 0)
	{
		printf("Resizing region memory %d -> %d.\n", m_numRegions, newSize);

		// Initialize memory for new regions
		auto oldRegionSize = sizeof(Region) * m_numRegions;
		auto newRegionSize = sizeof(Region) * (newSize - m_numRegions);
		memset(newRegions + m_numRegions, 0, newRegionSize);

		// If the array got moved, fixup next pointers in each
		// node to point into the new array
		if (newRegions != m_allRegions)
		{
			auto offset = (uintptr_t)newRegions - (uintptr_t)m_allRegions;
			for (auto i = 0; i < m_numRegions; i++)
			{
				auto region = &newRegions[i];
				if (region->m_next == nullptr)
					continue;

				region->m_next = (Region*)((uintptr_t)region->m_next + offset);
			}

			// Swap in new array, and fixup free and used array pointers too.
			m_allRegions = newRegions;
			m_freeRegions = (Region*)((uintptr_t)m_freeRegions + offset);
			m_usedRegions = (Region*)((uintptr_t)m_usedRegions + offset);
		}

		m_numRegions = newSize;
	}
	else
	{
		printf("CAN'T ALLOCATE ANY MORE REGION HANDLES!\n");
	}

	return 0;
}

void* RegionAllocator::allocate(uint32_t size, Gnm::AlignmentType alignment)
{
	// Give it a few chances before giving up.
	auto retry = 10;
	Region *r = NULL;
	while (true)
	{
		MUTEX_LOCK
			r = allocateRegion( size, kSystemRegionType, alignment);
		MUTEX_UNLOCK
			if (r != NULL)
				break;

		--retry;
		if (retry == 0)
		{
			printf("NATIVE ALLOCATOR EXAHUSTED!\n");
			return NULL;
		}

		// TODO: Re-add collection hint?
		//Game::DoManagedCollect();
	}

	if (r != NULL)
	{
		uint32_t *start = (uint32_t *)uintptr_t(r->m_typeAndStart & kRegionStartMask);
		// We've had reports of allocate*Memory() returning blocks that aren't fully within GPU-visible RAM.
		// Let's catch that!
#if GNM_MEMORY_DEBUG_OVERFLOW_CHECK==1
		SCE_GNM_ASSERT(alignment == r->m_guard);

		// Note: end may not be aligned to a word size!!
		uintptr_t start_u8 = (uintptr_t)start;
		uintptr_t end_u8   = start_u8 + (size + alignment);
		uint32_t  *end     = (uint32_t*)end_u8;

		for (uint32_t i=0; i<alignment/4; ++i)
		{
			start[i] = 0xdea110c8;
			end[i] = 0xdea110c8;
		}
		start += alignment/4;
#endif

		return start;
	}
	else
		return NULL;
}

void RegionAllocator::release(void* ptr)
{
	if(!ptr)
		return;

	uint64_t gpuAddr = uintptr_t(ptr);

	MUTEX_LOCK
		releaseRegion(gpuAddr, kSystemRegionType);
	MUTEX_UNLOCK
}

Region* RegionAllocator::allocateRegion(uint32_t size, uint64_t type, uint32_t alignment)
{
	// must be power of 2
	assert((alignment & (alignment-1)) == 0);

	uint32_t guard = 0;
#if GNM_MEMORY_DEBUG_OVERFLOW_CHECK==1
	guard = alignment;
	size += alignment * 2;
#endif

	Region *r, *rp = NULL;
	for (r = m_freeRegions; r; rp = r, r = r->m_next)
	{
		uint32_t miss;
			miss = static_cast<uint32_t>(r->m_typeAndStart & (alignment-1));
			if(miss != 0)
				miss = alignment - miss;

		if (r->m_size >= size + miss)
		{
			if (miss != 0)
			{
				// Align the region start
				Region *n = findUnusedRegion();
				if (n == NULL)
					break;

				n->m_size = miss;
				n->m_typeAndStart = r->m_typeAndStart;
				n->m_next = r;
				n->m_guard = 0;
				assert((r->m_typeAndStart & kRegionStartMask) + miss <= kRegionStartMask); // optimization check
				r->m_typeAndStart += miss;
				r->m_size -= miss;
				if (rp)
					rp->m_next = n;
				else
					m_freeRegions = n;
				rp = n;
			}

			uint64_t off = r->m_typeAndStart & kRegionStartMask;
			assert((r->m_typeAndStart & kRegionStartMask) + size <= kRegionStartMask); // optimization check
			r->m_typeAndStart += size;
			r->m_size -= size;
			assert((off & (alignment-1)) == 0);

			if (r->m_size == 0)
			{
				// Unlink me
				if (rp)
					rp->m_next = r->m_next;
				else
					m_freeRegions = r->m_next;

				// Re-link as used region
				Region *u = r;
				u->m_next = m_usedRegions;
				u->m_size = size;
				u->m_typeAndStart = off | type;
				u->m_guard = guard;
				m_usedRegions = u;
				//printf("a %08x %s %s\n", u->m_typeAndStart, humanSize(size), humanSize(alignment));
				m_totalMemoryInUse += size;
				return u;
			}
			else
			{
				// Link as used region
				Region *u = findUnusedRegion();
				if (u == NULL)
					break;

				u->m_next = m_usedRegions;
				u->m_size = size;
				u->m_guard = guard;
				u->m_typeAndStart = off | type;
				m_usedRegions = u;
				m_totalMemoryInUse += size;
				//printf("b %08x %s %s\n", u->m_typeAndStart, humanSize(size), humanSize(alignment));
				return u;
			}
		}
	}

	printf("ALLOC FAILED!\n");
	return NULL;
}

void RegionAllocator::releaseRegion(uint64_t start, uint64_t type)
{
	SCE_GNM_UNUSED(type);

	// Find the region in the used list
	Region *r, *rp = NULL;
	for (r = m_usedRegions; r; rp = r, r = r->m_next)
	{
		if ((r->m_typeAndStart & kRegionStartMask) == start)
		{
			assert((r->m_typeAndStart & kRegionTypeMask) == type);// "Released with different type than allocated with! %08x %08x", r->m_typeAndStart & kRegionTypeMask, type
			//printf("f: %08x %s\n", r->m_typeAndStart, humanSize(r->m_size));

			// Unlink
			if (rp)
				rp->m_next = r->m_next;
			else
				m_usedRegions = r->m_next;

			m_totalMemoryInUse -= r->m_size;
			uint64_t re = (r->m_typeAndStart & kRegionStartMask) + r->m_size;

			// Add to free list, find insertion point
			Region *f, *fp = NULL;
			for (f = m_freeRegions; f; fp = f, f = f->m_next)
			{
				if ((f->m_typeAndStart & kRegionStartMask) > start)
				{
					// Merge nearby blocks?
					if (fp && ((fp->m_typeAndStart & kRegionStartMask) + fp->m_size == (r->m_typeAndStart & kRegionStartMask)))
					{
						//printf("p\n");
						// Merge with previous
						fp->m_size += r->m_size;
						r->m_typeAndStart &= kRegionStartMask;

						// Merge prev with next?
						if ((fp->m_typeAndStart & kRegionStartMask) + fp->m_size == (f->m_typeAndStart & kRegionStartMask))
						{
							//printf("pn\n");
							fp->m_size += f->m_size;
							fp->m_next = f->m_next;
							f->m_typeAndStart &= kRegionStartMask;
						}
					}
					else if (re == (f->m_typeAndStart & kRegionStartMask))
					{
						//printf("n\n");
						// Merge with next
						f->m_typeAndStart -= r->m_size;
						f->m_size += r->m_size;
						r->m_typeAndStart &= kRegionStartMask;
					}
					else
					{
						// Insert
						r->m_next = f;
						if (fp)
							fp->m_next = r;
						else
							m_freeRegions = r;
					}
					break;
				}
			}

			if (f == NULL)
			{
				// Add to end
				r->m_next = NULL;
				if (fp)
					fp->m_next = r;
				else
					m_freeRegions = r;
			}

			return;
		}
	}

	fprintf(stderr, "Pointer 0x%p was not allocated through the allocator!\n", start);
	assert(!"Pointer not allocator with this allocator");
}

// Allocator
Allocator Allocator::_instance;

Allocator* Allocator::Get()
{
	if (!_instance._isInitialized)
	{
		_instance.initialize();
		_instance._isInitialized = true;
	}

	return &_instance;
}

Allocator::Allocator()
	:	m_onionMemAddr(0), m_garlicMemAddr(0),
	m_onionMemSize(0), m_garlicMemSize(0),
	m_onionAllocator(), m_garlicAllocator()
{
}

void Allocator::initialize()
{
	mapMemory(
		&m_onionMemAddr, &m_onionMemSize,
		&m_garlicMemAddr, &m_garlicMemSize);

	m_onionAllocator.init(m_onionMemAddr, m_onionMemSize, kInitialOnionRegions);
	m_garlicAllocator.init(m_garlicMemAddr, m_garlicMemSize, kInitialGarlicRegions);
}

void* Allocator::allocate(size_t size, size_t align, SceKernelMemoryType type)
{
	assert(size > 0);
	assert(align > 0);

	void* ret = 0;

	switch( type )
	{
	case SCE_KERNEL_WB_ONION:
		ret = m_onionAllocator.allocate(size, align);
#ifndef NDEBUG
		//printf("Total System: %s\n", humanSize(m_onionAllocator.getMemoryUsage()));
#endif
		return ret;

	case SCE_KERNEL_WC_GARLIC:
		ret = m_garlicAllocator.allocate(size, align);
#ifndef NDEBUG
		//printf("Total Shared:  %s\n", humanSize(m_garlicAllocator.getMemoryUsage()));
#endif
		return ret;

	default:
		return NULL;
	}
}

void* Allocator::allocate(sce::Gnm::SizeAlign sizeAlign, SceKernelMemoryType type)
{
	return allocate(sizeAlign.m_size, sizeAlign.m_align, type);
}

void Allocator::release(void *ptr)
{
	if( !ptr )
		return;

	uint64_t address = reinterpret_cast<uint64_t>(ptr);
	if( address >= m_onionMemAddr && address < m_onionMemAddr+m_onionMemSize )
	{
		m_onionAllocator.release(ptr);
	}
	else if( address >= m_garlicMemAddr && address < m_garlicMemAddr+m_garlicMemSize )
	{
		m_garlicAllocator.release(ptr);
	}
	else
	{
		printf("Unknown memory range: 0x%016lX\n", address);
		assert(!"Unknown memory pointer");
	}
}

namespace mem
{
	void* alloc(size_t size, uint32_t align)
	{
		return Allocator::Get()->allocate(size, align, SCE_KERNEL_WB_ONION);
	}

	void free(void* ptr)
	{
		Allocator::Get()->release(ptr);
	}

	void* allocShared(size_t size, uint32_t align)
	{
		return Allocator::Get()->allocate(size, align, SCE_KERNEL_WC_GARLIC); 
	}

	void* allocShared(Gnm::SizeAlign sizeAlign)
	{
		return Allocator::Get()->allocate(sizeAlign, SCE_KERNEL_WC_GARLIC);
	}

	void freeShared(void* ptr)
	{
		return Allocator::Get()->release(ptr);
	}
}
