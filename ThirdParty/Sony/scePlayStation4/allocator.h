#ifndef _ALLOCATOR_H_
#define _ALLOCATOR_H_

#include <gnm/common.h>
#include <sys/dmem.h>
#include <kernel.h>
#include <gnm/gpumem.h>
#include <memory.h>


class Region
{
public:
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

    Region* findUnusedRegion() const;
    Region* allocateRegion(uint32_t size, uint64_t type, uint32_t alignment);
    void	releaseRegion(uint64_t start, uint64_t type);
public:
    void	init(uint64_t memStart, uint32_t memSize, Region* regions, uint32_t numRegions);

    void*	allocate(uint32_t size, sce::Gnm::AlignmentType alignment);
    void*	allocate(sce::Gnm::SizeAlign sz) { return allocate(sz.m_size, sz.m_align); }
    void	release(void*);
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

    static Allocator *_instance;

    uint64_t m_onionMemAddr;
    uint64_t m_garlicMemAddr;
    uint32_t m_onionMemSize;
    uint32_t m_garlicMemSize;

    RegionAllocator m_onionAllocator;
    RegionAllocator m_garlicAllocator;

    // These are the number of "handles" for memory
    // allocations, if you get an out or regions
    // crash then this is what you need to increase.
    Region m_onionRegions[8192];
    Region m_garlicRegions[3072];
};


#endif // _ALLOCATOR_H_
