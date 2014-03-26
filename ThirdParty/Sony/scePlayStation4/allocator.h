/* SCE CONFIDENTIAL
ORBIS Programmer Tool Runtime Library Release 01.020.041
* Copyright (C) 2013 Sony Computer Entertainment Inc.
* All Rights Reserved.
*/

#ifndef _ALLOCATOR_H_
#define _ALLOCATOR_H_

#include <gnm/common.h>
#include <sys/dmem.h>
#include <kernel.h>
#include <gnm/gpumem.h>
#include "memory.h"

namespace TFPS4
{
    static const uint64_t kRegionTypeMask = 0xE000000000000000ULL;
    static const uint64_t kRegionStartMask = 0x1FFFFFFFFFFFFFFFULL;

    static const uint64_t kUnusedRegionType      = 0x0000000000000000ULL;
    static const uint64_t kSystemRegionType      = 0x2000000000000000ULL;
    static const uint64_t kSharedVideoRegionType = 0x4000000000000000ULL;
    static const uint64_t kPrivateVideoRegionType= 0x8000000000000000ULL;

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
        //SceKernelMemoryType m_type;
        Region *m_freeRegions;
        Region *m_usedRegions;
        Region *m_allRegions;
        uint32_t m_numRegions;
        uint64_t m_memoryStart;
        uint64_t m_memoryEnd;

        Region* findUnusedRegion() const;
        Region* allocateRegion(uint32_t size, uint64_t type, uint32_t alignment);
        void	releaseRegion(uint64_t start, uint64_t type);
    public:
        void	init(uint64_t memStart, uint32_t memSize, Region* regions, uint32_t numRegions);

        void*	allocate(uint32_t size, sce::Gnm::AlignmentType alignment);
        void*	allocate(sce::Gnm::SizeAlign sz) { return allocate(sz.m_size, sz.m_align); }
        void	release(void*);
    };

    /** @brief Maps system and shared memory.
    @param system         The base system memory address.
    @param systemSize     The size of the system memory window.
    @param gpuShared      The base shared video memory address.
    @param gpuSharedSize  The size of the shared video memory window.
    */
    void mapMemory(	uint64_t	*system,		uint32_t* systemSize,
        uint64_t	*gpuShared,		uint32_t* gpuSharedSize);

    /*
    #if GNM_MEMORY_STORE_ALLOC_FILE
    #ifndef GNM_MEMORY_STORE_INHIB_MACRO
    #define allocateSystemSharedMemory(...)  recordFileNameAndLineInRegion(Toolkit::allocateSystemSharedMemory(__VA_ARGS__), __FILE__, __LINE__)
    #define allocateVideoSharedMemory(...)   recordFileNameAndLineInRegion(Toolkit::allocateVideoSharedMemory(__VA_ARGS__), __FILE__, __LINE__)
    #endif // GNM_MEMORY_STORE_INHIB_MACRO

    #endif // GNM_MEMORY_STORE_ALLOC_FILE


    IAllocator GetInterface(RegionAllocator *regionAllocator);

    typedef RegionAllocator Allocator; // THIS EXISTS TEMPORARILY, AND ONLY BECAUSE THE OLD "Allocator" IS NOW CALLED "RegionAllocator"
    */
}

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


    static Allocator *_instance;

    uint64_t m_onionMemAddr;
    uint64_t m_garlicMemAddr;
    uint32_t m_onionMemSize;
    uint32_t m_garlicMemSize;

    TFPS4::RegionAllocator m_onionAllocator;
    TFPS4::RegionAllocator m_garlicAllocator;

    TFPS4::Region m_onionRegions[1024];
    TFPS4::Region m_garlicRegions[2048];
};

#endif
