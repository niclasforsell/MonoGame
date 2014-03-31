#include "Allocator.h"

#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kernel.h>

// RegionAllocator
using namespace sce;

#define DISABLE_MEMORY_CLEAR

#define GNM_MEMORY_DEBUG_OVERFLOW_CHECK 0
#define GNM_MEMORY_DEBUG_ALIGNMENT_CHECK 0


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

void TFPS4::mapMemory(uint64_t* system, uint32_t* systemSize, uint64_t* gpuShared, uint32_t* gpuSharedSize)
{
    assert(system && systemSize && gpuShared && gpuSharedSize);

    // Setup the memory pools:
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

    fprintf(stderr,"Total Direct Memory         : %.2fMB\n", static_cast<uint64_t>(SCE_KERNEL_MAIN_DMEM_SIZE) / 1024.0f / 1024.0f);
    fprintf(stderr,"Allocator System/Onion Size : %.2fMB\n", static_cast<uint64_t>(systemPoolSize) / 1024.0f / 1024.0f);
    fprintf(stderr,"Allocator Video/Garlic Size : %.2fMB\n", static_cast<uint64_t>(sharedGpuPoolSize) / 1024.0f / 1024.0f);
}


void TFPS4::RegionAllocator::init(uint64_t memStart, uint32_t memSize, Region* regions, uint32_t numRegions)
{
    m_memoryStart = memStart;
    m_memoryEnd = memStart + memSize;
    m_allRegions = regions;
    m_numRegions = numRegions;
    m_freeRegions = NULL;
    m_usedRegions = NULL;

    auto ret = scePthreadMutexInit(&m_lock, NULL, NULL);
    if (ret) 
        printf("scePthreadMutexInit failed: 0x%08X\n", ret);

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

TFPS4::Region* TFPS4::RegionAllocator::findUnusedRegion() const
{
    for (uint32_t i = 0; i < m_numRegions; ++i)
    {
        Region *r = &m_allRegions[i];
        if ((r->m_typeAndStart & kRegionTypeMask) == kUnusedRegionType)
            return r;
    }
    return 0;
}

void* TFPS4::RegionAllocator::allocate(uint32_t size, Gnm::AlignmentType alignment)
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

void TFPS4::RegionAllocator::release(void* ptr)
{
    if(!ptr)
        return;

    uint64_t gpuAddr = uintptr_t(ptr);

    MUTEX_LOCK
        releaseRegion(gpuAddr, kSystemRegionType);
    MUTEX_UNLOCK
}

TFPS4::Region* TFPS4::RegionAllocator::allocateRegion(uint32_t size, uint64_t type, uint32_t alignment)
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
        // the following code ensures that all allocations with alignment<512 are aligned to 512, skipped
        // forward by 1024, then skipped back by the requested alignment. this ensures MISALIGNMENT
        // to (alignment..512] which makes alignment-truncated addresses point as far backwards
        // into memory "landfill" as is practical.

        uint32_t miss;
#if GNM_MEMORY_DEBUG_ALIGNMENT_CHECK==1
        enum { kMaximumIntentionalMisalignment = 256 };
        if( alignment < kMaximumIntentionalMisalignment*2 )
        {
            const uint32_t begin = (uint32_t)(r->m_typeAndStart & kRegionStartMask);
            uint32_t end = begin;
            end &= ~(kMaximumIntentionalMisalignment*2-1); // align to 512
            end +=   kMaximumIntentionalMisalignment*2*2;  // skip forward by 1024
            end -= alignment; // subtract desired alignment
            miss = end - begin; // calculate difference
        }
        else
#endif
        {
            miss = static_cast<uint32_t>(r->m_typeAndStart & (alignment-1));
            if(miss != 0)
                miss = alignment - miss;
        }

        if (r->m_size >= size + miss)
        {
            if (miss != 0)
            {
                // Align the region start
                Region *n = findUnusedRegion();
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
                //printf("a %08x %08x %08x\n", u->m_typeAndStart, size, alignment);
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
                //printf("b %08x %08x %08x\n", u->m_typeAndStart, size, alignment);
                return u;
            }
        }
    }

    printf("ALLOC FAILED!");
    return NULL;
}

void TFPS4::RegionAllocator::releaseRegion(uint64_t start, uint64_t type)
{
    SCE_GNM_UNUSED(type);

    // Find the region in the used list
    Region *r, *rp = NULL;
    for (r = m_usedRegions; r; rp = r, r = r->m_next)
    {
        if ((r->m_typeAndStart & kRegionStartMask) == start)
        {
            assert((r->m_typeAndStart & kRegionTypeMask) == type);// "Released with different type than allocated with! %08x %08x", r->m_typeAndStart & kRegionTypeMask, type
            //printf("f: %08x %08x\n", r->m_typeAndStart, r->m_size);

            // Unlink
            if (rp)
                rp->m_next = r->m_next;
            else
                m_usedRegions = r->m_next;

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

    assert(!"Pointer Not Allocated With GNM");
}

// Allocator
Allocator *Allocator::_instance = NULL;

Allocator* Allocator::Get()
{
    if (_instance == NULL)
    {
        _instance = new Allocator();
        _instance->initialize();
    }

    return _instance;
}

Allocator::Allocator()
    :	m_onionMemAddr(0), m_garlicMemAddr(0),
    m_onionMemSize(0), m_garlicMemSize(0),
    m_onionAllocator(), m_garlicAllocator()
{
    memset(m_onionRegions, 0, sizeof(m_onionRegions));
    memset(m_garlicRegions, 0, sizeof(m_garlicRegions));
}

void Allocator::initialize()
{
    TFPS4::mapMemory(
        &m_onionMemAddr, &m_onionMemSize,
        &m_garlicMemAddr, &m_garlicMemSize);

    m_onionAllocator.init(
        m_onionMemAddr, m_onionMemSize,
        m_onionRegions, 1024);

    m_garlicAllocator.init(
        m_garlicMemAddr, m_garlicMemSize,
        m_garlicRegions, 2048);
}

void* Allocator::allocate(size_t size, size_t align, SceKernelMemoryType type)
{
	assert(size > 0);
	assert(align > 0);

    switch( type )
    {
    case SCE_KERNEL_WB_ONION:
        return m_onionAllocator.allocate(size, align);

    case SCE_KERNEL_WC_GARLIC:
        return m_garlicAllocator.allocate(size, align);

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
    }
}
