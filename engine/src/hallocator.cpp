#include "hallocator.h"
#include "hshared.h"
#include <stdlib.h>
#include <dlfcn.h>
#include "hgenericexception.h"
#include "rpmalloc.h"

namespace hf
{
    struct AllocatorData
    {
        std::atomic<bool> isInitialized = false;
    };

    AllocatorData ALLOCATOR_DATA;

    double ToMB(size_t bytes)
    {
        return bytes * (1.0 / (1024.0 * 1024.0));
    }

    void LoadAllocator()
    {
        if (!ALLOCATOR_DATA.isInitialized)
        {
            rpmalloc_initialize();
            rpmalloc_thread_initialize();
            ALLOCATOR_DATA.isInitialized = true;
        }
    }

    void UnloadAllocator()
    {
        if (ALLOCATOR_DATA.isInitialized)
        {
            rpmalloc_thread_finalize(1);
            rpmalloc_finalize();
            ALLOCATOR_DATA.isInitialized = false;
        }
    }

    void LoadAllocatorThread()
    {
        if (ALLOCATOR_DATA.isInitialized && !rpmalloc_is_thread_initialized()) rpmalloc_thread_initialize();
    }

    void UnloadAllocatorThread()
    {
        if (ALLOCATOR_DATA.isInitialized && rpmalloc_is_thread_initialized()) rpmalloc_thread_finalize(true);
    }

    void LogMemoryStats()
    {
        rpmalloc_global_statistics_t sts{};
        rpmalloc_global_statistics(&sts);
        LOG_LOG("\n[Hyperflow] Memory Stats In MB:\nMapped: %0.2g\nMapped Peak: %0.2g\nCashed: %0.2g\nHuge Alloc: %0.2g\nHuge Alloc Peak: %0.2g\nMapped Total: %0.2g\nUnmapped Total: %0.2g",
            ToMB(sts.mapped), ToMB(sts.mapped_peak), ToMB(sts.cached), ToMB(sts.huge_alloc), ToMB(sts.huge_alloc_peak), ToMB(sts.mapped_total), ToMB(sts.unmapped_total));
    }

    void LogThreadMemoryStats()
    {
        rpmalloc_thread_statistics_t sts{};
        rpmalloc_thread_statistics(&sts);
        LOG_LOG("\n[Hyperflow] Thread Memory Stats in MB:\nCash Size: %0.2g\nCash Span: %0.2g\nThread To Global: %0.2g\nGlobal To Thread: %0.2g",
            ToMB(sts.sizecache), ToMB(sts.spancache), ToMB(sts.thread_to_global), ToMB(sts.global_to_thread));

        auto& sp = *sts.span_use;
        LOG_LOG("\n[Span Use]\nCurrent: %0.2g\nPeak: %0.2g\nTo Global: %0.2g\nFrom Global: %0.2g\nTo Cache: %0.2g\nFrom Cache: %0.2g\nTo Reserved: %0.2g\nFrom Reserved: %0.2g\nMap Calls: %i",
            ToMB(sp.current), ToMB(sp.peak), ToMB(sp.to_global), ToMB(sp.from_global), ToMB(sp.to_cache), ToMB(sp.from_cache), ToMB(sp.to_reserved), ToMB(sp.from_reserved), sp.map_calls);

        auto& su = *sts.size_use;
        LOG_LOG("\n[Size Use]\nAlloc Current: %0.2g\nAlloc Peak: %0.2g\nAlloc Total: %0.2g\nFree Total: %0.2g\nSpans To Cache: %0.2g\nSpans From Cache: %0.2g\nSpans From Reserved: %0.2g\nMap Calls: %i",
            ToMB(su.alloc_current), ToMB(su.alloc_peak), ToMB(su.alloc_total), ToMB(su.free_total), ToMB(su.spans_to_cache), ToMB(su.spans_from_cache), ToMB(su.spans_from_reserved), su.map_calls);
    }
}

void* operator new(std::size_t size)
{
#if DEBUG
    hf::LoadAllocatorThread();
#endif
    void* memory = rpmalloc(size);
    if (!memory) throw std::bad_alloc();
    return memory;
}

void operator delete(void* ptr) noexcept
{
    rpfree(ptr);
}

void* operator new[](std::size_t size)
{
#if DEBUG
    hf::LoadAllocatorThread();
#endif

    void* memory = rpmalloc(size);
    if (!memory) throw std::bad_alloc();
    return memory;
}

void operator delete[](void* ptr) noexcept
{
    rpfree(ptr);
}

// void* malloc(size_t size) noexcept
// {
//     static void* (*system_malloc)(size_t) = (void* (*)(size_t))dlsym(RTLD_NEXT, "malloc");
//
// #if DEBUG
//     hf::LoadAllocatorThread();
// #endif
//     void* memory = hf::ALLOCATOR_DATA.isInitialized ? rpmalloc(size) : system_malloc(size);
//
//     if (!memory) fprintf(stderr, "Unable to allocate memory");
//     return memory;
// }
//
// void free(void* ptr)
// {
//     rpfree(ptr);
// }
//
// void* calloc(size_t num, size_t size) noexcept
// {
//     static void* (*system_calloc)(size_t, size_t) = (void* (*)(size_t, size_t))dlsym(RTLD_NEXT, "calloc");
//
// #if DEBUG
//     hf::LoadAllocatorThread();
// #endif
//     void* memory = hf::ALLOCATOR_DATA.isInitialized ? rpcalloc(num, size) : system_calloc(num, size);
//     if (!memory) fprintf(stderr, "Unable to allocate memory");
//     return memory;
// }
//
// void* realloc(void* ptr, size_t size) noexcept
// {
//     static void* (*system_realloc)(void*, size_t) = (void* (*)(void*, size_t))dlsym(RTLD_NEXT, "realloc");
// #if DEBUG
//     hf::LoadAllocatorThread();
// #endif
//     void* memory = hf::ALLOCATOR_DATA.isInitialized ? rprealloc(ptr, size) : system_realloc(ptr, size);
//     if (!memory) fprintf(stderr, "Unable to allocate memory");
//     return memory;
// }