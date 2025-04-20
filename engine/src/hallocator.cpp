#include "hallocator.h"
#include "hshared.h"
#include "hgenericexception.h"
#include "rpmalloc.h"

namespace hf
{
    double ToMB(size_t bytes)
    {
        return bytes * (1.0 / (1024.0 * 1024.0));
    }

    void LoadAllocator()
    {
        rpmalloc_initialize();
        rpmalloc_thread_initialize();
    }

    void UnloadAllocator()
    {
#if DEBUG
        LogMemoryStats();
#endif
        rpmalloc_thread_finalize(true);
        rpmalloc_finalize();
    }

    void LoadAllocatorThread()
    {
        if (!rpmalloc_is_thread_initialized()) rpmalloc_thread_initialize();
    }

    void UnloadAllocatorThread()
    {
        if (rpmalloc_is_thread_initialized()) rpmalloc_thread_finalize(true);
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