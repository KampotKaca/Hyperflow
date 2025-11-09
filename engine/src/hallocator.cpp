#include "hshared.h"
#include "hgenericexception.h"
#include "hinternal.h"
#include "rpmalloc.h"
#include "hyperflow.h"

#ifdef HF_ENABLE_ALLOCATOR
#include <stdexcept>
#endif

namespace hf
{
    namespace ir::alloc
    {
        double ToMB(size_t bytes)
        {
            return (double)bytes * (1.0 / (1024.0 * 1024.0));
        }

        void UnloadAllocator_i()
        {
            rpmalloc_thread_finalize(1);
            rpmalloc_finalize();
        }

        void LoadAllocatorThread_i()
        {
            rpmalloc_thread_initialize();
        }

        void UnloadAllocatorThread_i()
        {
            if (rpmalloc_is_thread_initialized()) rpmalloc_thread_finalize(true);
        }

        void LogMemoryStats_i()
        {
            rpmalloc_global_statistics_t sts{};
            rpmalloc_global_statistics(&sts);
            log_log("\n[Hyperflow] Memory Stats In MB:\nMapped: %0.2g\nMapped Peak: %0.2g\nCashed: %0.2g\nHuge Alloc: %0.2g\nHuge Alloc Peak: %0.2g\nMapped Total: %0.2g\nUnmapped Total: %0.2g",
                ToMB(sts.mapped), ToMB(sts.mapped_peak), ToMB(sts.cached), ToMB(sts.huge_alloc), ToMB(sts.huge_alloc_peak), ToMB(sts.mapped_total), ToMB(sts.unmapped_total));
        }

        void LogThreadMemoryStats_i()
        {
            rpmalloc_thread_statistics_t sts{};
            rpmalloc_thread_statistics(&sts);
            log_log("\n[Hyperflow] Thread Memory Stats in MB:\nCash Size: %0.2g\nCash Span: %0.2g\nThread To Global: %0.2g\nGlobal To Thread: %0.2g",
                ToMB(sts.sizecache), ToMB(sts.spancache), ToMB(sts.thread_to_global), ToMB(sts.global_to_thread));

            const auto& sp = *sts.span_use;
            log_log("\n[Span Use]\nCurrent: %0.2g\nPeak: %0.2g\nTo Global: %0.2g\nFrom Global: %0.2g\nTo Cache: %0.2g\nFrom Cache: %0.2g\nTo Reserved: %0.2g\nFrom Reserved: %0.2g\nMap Calls: %i",
                sp.current, sp.peak, sp.to_global, sp.from_global, sp.to_cache, sp.from_cache, sp.to_reserved, sp.from_reserved, sp.map_calls);

            const auto& su = *sts.size_use;
            log_log("\n[Size Use]\nAlloc Current: %0.2g\nAlloc Peak: %0.2g\nAlloc Total: %0.2g\nFree Total: %0.2g\nSpans To Cache: %0.2g\nSpans From Cache: %0.2g\nSpans From Reserved: %0.2g\nMap Calls: %i",
                su.alloc_current, su.alloc_peak, su.alloc_total, su.free_total, su.spans_to_cache, su.spans_from_cache, su.spans_from_reserved, su.map_calls);
        }
    }

    namespace utils
    {
        GlobalMemoryStatistics GetGlobalMemoryStatistics()
        {
            rpmalloc_global_statistics_t sts{};
            rpmalloc_global_statistics(&sts);

            GlobalMemoryStatistics stats{};
            stats.mappedSizeMbs        = ir::alloc::ToMB(sts.mapped);
            stats.mappedPeakSizeMbs    = ir::alloc::ToMB(sts.mapped_peak);
            stats.cachedSizeMbs        = ir::alloc::ToMB(sts.cached);
            stats.hugeAllocSizeMbs     = ir::alloc::ToMB(sts.huge_alloc);
            stats.hugeAllocPeakSizeMbs = ir::alloc::ToMB(sts.huge_alloc_peak);
            stats.mappedTotalSizeMbs   = ir::alloc::ToMB(sts.mapped_total);
            stats.unmappedTotalSizeMbs = ir::alloc::ToMB(sts.unmapped_total);
            return stats;
        }

        ThreadMemoryStatistics GetThreadMemoryStatistics()
        {
            rpmalloc_thread_statistics_t sts{};
            rpmalloc_thread_statistics(&sts);
            const auto& sp = *sts.span_use;
            const auto& su = *sts.size_use;

            ThreadMemoryStatistics stats{};
            stats.cacheSizeMbs      = ir::alloc::ToMB(sts.sizecache);
            stats.cacheSpanMbs      = ir::alloc::ToMB(sts.spancache);
            stats.threadToGlobalMbs = ir::alloc::ToMB(sts.thread_to_global);
            stats.globalToThreadMbs = ir::alloc::ToMB(sts.global_to_thread);

            stats.currentNumSpans = sp.current;
            stats.peakNumSpans    = sp.peak;

            stats.currentNumAllocations = su.alloc_current;
            stats.peakNumAllocations    = su.alloc_peak;
            stats.totalNumAllocations   = su.alloc_total;
            stats.totalNumFrees         = su.free_total;
            return stats;
        }

        RendererStatistics GetRendererStatistics() { return ir::HF.renderer->lastDrawStatistics; }
    }
}

#if defined(HF_ENABLE_ALLOCATOR)

///////////////////////////////////////////////////////////////////////
// USUAL OVERLOADS
void* operator new(std::size_t size)
{
    return hf::utils::Alloc(size);
}

void operator delete(void* ptr)
{
    hf::utils::Free(ptr);
}

void* operator new[](std::size_t size)
{
    return hf::utils::Alloc(size);
}

void operator delete[](void* ptr) noexcept
{
    hf::utils::Free(ptr);
}

///////////////////////////////////////////////////////////////////////
// WITH std::nothrow_t
void* operator new(std::size_t size, const std::nothrow_t&) noexcept
{
    return hf::utils::Alloc(size);
}

void operator delete(void* ptr, const std::nothrow_t&) noexcept
{
    hf::utils::Free(ptr);
}

void* operator new[](std::size_t size, const std::nothrow_t&) noexcept
{
    return hf::utils::Alloc(size);
}

void operator delete[](void* ptr, const std::nothrow_t&) noexcept
{
    hf::utils::Free(ptr);
}

///////////////////////////////////////////////////////////////////////
// WITH ALIGNMENT
void* operator new(std::size_t size, std::align_val_t alignment)
{
    return hf::utils::AllocAligned(size, static_cast<std::size_t>(alignment));
}

void operator delete(void* ptr, std::align_val_t alignment) noexcept
{
    hf::utils::Free(ptr);
}

void* operator new[](std::size_t size, std::align_val_t alignment)
{
    return hf::utils::AllocAligned(size, static_cast<std::size_t>(alignment));
}

void operator delete[](void* ptr, std::align_val_t alignment) noexcept
{
    hf::utils::Free(ptr);
}

///////////////////////////////////////////////////////////////////////
// WITH ALIGNMENT std::size_t
void* operator new(std::size_t size, std::size_t alignment)
{
    return hf::utils::AllocAligned(size, alignment);
}

void* operator new[](std::size_t size, std::size_t alignment)
{
    return hf::utils::AllocAligned(size, alignment);
}

///////////////////////////////////////////////////////////////////////
// WITH ALIGNMENT and std::nothrow_t
void* operator new(std::size_t size, std::align_val_t alignment, const std::nothrow_t& tag) noexcept
{
    return hf::utils::AllocAligned(size, static_cast<std::size_t>(alignment));
}

void* operator new[](std::size_t size, std::align_val_t alignment, const std::nothrow_t& tag) noexcept
{
    return hf::utils::AllocAligned(size, static_cast<std::size_t>(alignment));
}

void operator delete(void* ptr, std::align_val_t, const std::nothrow_t &) noexcept
{
    hf::utils::Free(ptr);
}

void operator delete[](void* ptr, std::align_val_t, const std::nothrow_t &) noexcept
{
    hf::utils::Free(ptr);
}

///////////////////////////////////////////////////////////////////////
// WITH ALIGNMENT and std::nothrow_t & std::size_t alignment not std::align_val_t
void* operator new(std::size_t size, std::size_t alignment, const std::nothrow_t& tag) noexcept
{
    return hf::utils::AllocAligned(size, alignment);
}

void* operator new[](std::size_t size, std::size_t alignment, const std::nothrow_t& tag) noexcept
{
    return hf::utils::AllocAligned(size, alignment);
}

void operator delete(void* ptr, std::size_t, const std::nothrow_t &) noexcept
{
    hf::utils::Free(ptr);
}

void operator delete[](void* ptr, std::size_t, const std::nothrow_t &) noexcept
{
    hf::utils::Free(ptr);
}

///////////////////////////////////////////////////////////////////////
// DELETES WITH SIZES
void operator delete(void* ptr, std::size_t size) noexcept
{
    hf::utils::Free(ptr);
}

void operator delete[](void* ptr, std::size_t size) noexcept
{
    hf::utils::Free(ptr);
}

void operator delete(void* ptr, std::size_t size, std::align_val_t align) noexcept
{
    hf::utils::Free(ptr);
}

void operator delete[](void* ptr, std::size_t size, std::align_val_t align) noexcept
{
    hf::utils::Free(ptr);
}

void operator delete(void* ptr, std::size_t size, std::size_t align) noexcept
{
    hf::utils::Free(ptr);
}

void operator delete[](void* ptr, std::size_t size, std::size_t align) noexcept
{
    hf::utils::Free(ptr);
}

#endif