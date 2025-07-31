#include "hshared.h"
#include "hgenericexception.h"
#include "hinternal.h"
#include "rpmalloc.h"
#include "hyperflow.h"

namespace hf
{
    namespace utils
    {
        static std::once_flag ALLOCATOR_INIT_FLAG;

        static void EnsureAllocatorInit()
        {
            std::call_once(ALLOCATOR_INIT_FLAG, []
            {
                rpmalloc_initialize();
                rpmalloc_thread_initialize();
            });
        }

        void* Allocate(std::size_t n)
        {
            EnsureAllocatorInit();
            inter::alloc::LoadAllocatorThread_i();

            void* memory = rpmalloc(n);
            if (!memory) throw std::bad_alloc();
            return memory;
        }

        void* AllocateAligned(std::size_t n, std::align_val_t align)
        {
            EnsureAllocatorInit();
            inter::alloc::LoadAllocatorThread_i();
            void* memory = rpaligned_alloc((size_t)align, n);
            if (!memory) throw std::bad_alloc();
            return memory;
        }

        void Deallocate(void* p)
        {
            rpfree(p);
        }

        void DeallocateAligned(void* p, std::align_val_t align)
        {
            rpfree(p);
        }
    }

    namespace inter::alloc
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
            if (!rpmalloc_is_thread_initialized()) rpmalloc_thread_initialize();
        }

        void UnloadAllocatorThread_i()
        {
            if (rpmalloc_is_thread_initialized()) rpmalloc_thread_finalize(true);
        }

        void LogMemoryStats_i()
        {
            rpmalloc_global_statistics_t sts{};
            rpmalloc_global_statistics(&sts);
            LOG_LOG("\n[Hyperflow] Memory Stats In MB:\nMapped: %0.2g\nMapped Peak: %0.2g\nCashed: %0.2g\nHuge Alloc: %0.2g\nHuge Alloc Peak: %0.2g\nMapped Total: %0.2g\nUnmapped Total: %0.2g",
                ToMB(sts.mapped), ToMB(sts.mapped_peak), ToMB(sts.cached), ToMB(sts.huge_alloc), ToMB(sts.huge_alloc_peak), ToMB(sts.mapped_total), ToMB(sts.unmapped_total));
        }

        void LogThreadMemoryStats_i()
        {
            rpmalloc_thread_statistics_t sts{};
            rpmalloc_thread_statistics(&sts);
            LOG_LOG("\n[Hyperflow] Thread Memory Stats in MB:\nCash Size: %0.2g\nCash Span: %0.2g\nThread To Global: %0.2g\nGlobal To Thread: %0.2g",
                ToMB(sts.sizecache), ToMB(sts.spancache), ToMB(sts.thread_to_global), ToMB(sts.global_to_thread));

            const auto& sp = *sts.span_use;
            LOG_LOG("\n[Span Use]\nCurrent: %0.2g\nPeak: %0.2g\nTo Global: %0.2g\nFrom Global: %0.2g\nTo Cache: %0.2g\nFrom Cache: %0.2g\nTo Reserved: %0.2g\nFrom Reserved: %0.2g\nMap Calls: %i",
                sp.current, sp.peak, sp.to_global, sp.from_global, sp.to_cache, sp.from_cache, sp.to_reserved, sp.from_reserved, sp.map_calls);

            const auto& su = *sts.size_use;
            LOG_LOG("\n[Size Use]\nAlloc Current: %0.2g\nAlloc Peak: %0.2g\nAlloc Total: %0.2g\nFree Total: %0.2g\nSpans To Cache: %0.2g\nSpans From Cache: %0.2g\nSpans From Reserved: %0.2g\nMap Calls: %i",
                su.alloc_current, su.alloc_peak, su.alloc_total, su.free_total, su.spans_to_cache, su.spans_from_cache, su.spans_from_reserved, su.map_calls);
        }

        GlobalMemoryStatistics GetGlobalMemoryStatistics_i()
        {
            rpmalloc_global_statistics_t sts{};
            rpmalloc_global_statistics(&sts);

            GlobalMemoryStatistics stats{};
            stats.mappedSizeMbs        = ToMB(sts.mapped);
            stats.mappedPeakSizeMbs    = ToMB(sts.mapped_peak);
            stats.cashedSizeMbs        = ToMB(sts.cached);
            stats.hugeAllocSizeMbs     = ToMB(sts.huge_alloc);
            stats.hugeAllocPeakSizeMbs = ToMB(sts.huge_alloc_peak);
            stats.mappedTotalSizeMbs   = ToMB(sts.mapped_total);
            stats.unmappedTotalSizeMbs = ToMB(sts.unmapped_total);
            return stats;
        }

        ThreadMemoryStatistics GetThreadMemoryStatistics_i()
        {
            rpmalloc_thread_statistics_t sts{};
            rpmalloc_thread_statistics(&sts);
            const auto& sp = *sts.span_use;
            const auto& su = *sts.size_use;

            ThreadMemoryStatistics stats{};
            stats.cacheSizeMbs      = ToMB(sts.sizecache);
            stats.cacheSpanMbs      = ToMB(sts.spancache);
            stats.threadToGlobalMbs = ToMB(sts.thread_to_global);
            stats.globalToThreadMbs = ToMB(sts.global_to_thread);

            stats.currentNumSpans = sp.current;
            stats.peakNumSpans    = sp.peak;

            stats.currentNumAllocations = su.alloc_current;
            stats.peakNumAllocations    = su.alloc_peak;
            stats.totalNumAllocations   = su.alloc_total;
            stats.totalNumFrees         = su.free_total;
            return stats;
        }
    }
}

void* operator new(std::size_t size)
{
    return hf::utils::Allocate(size);
}

void* operator new[](std::size_t size)
{
    return hf::utils::Allocate(size);
}

void* operator new(std::size_t size, const std::nothrow_t&) noexcept
{
    return hf::utils::Allocate(size);
}

void* operator new[](std::size_t size, const std::nothrow_t&) noexcept
{
    return hf::utils::Allocate(size);
}

void* operator new(std::size_t size, std::align_val_t align)
{
    return hf::utils::AllocateAligned(size, align);
}

void* operator new[](std::size_t size, std::align_val_t align)
{
    return hf::utils::AllocateAligned(size, align);
}

void* operator new(std::size_t size, std::align_val_t align, const std::nothrow_t&) noexcept
{
    return hf::utils::AllocateAligned(size, align);
}

void* operator new[](std::size_t size, std::align_val_t align, const std::nothrow_t&) noexcept
{
    return hf::utils::AllocateAligned(size, align);
}

void operator delete(void* ptr) noexcept
{
    hf::utils::Deallocate(ptr);
}

void operator delete[](void* ptr) noexcept
{
    hf::utils::Deallocate(ptr);
}

void operator delete(void* ptr, std::size_t) noexcept
{
    hf::utils::Deallocate(ptr);
}

void operator delete[](void* ptr, std::size_t) noexcept
{
    hf::utils::Deallocate(ptr);
}

void operator delete(void* ptr, const std::nothrow_t&) noexcept
{
    hf::utils::Deallocate(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t&) noexcept
{
    hf::utils::Deallocate(ptr);
}

void operator delete(void* ptr, std::size_t size, std::align_val_t align) noexcept
{
    hf::utils::DeallocateAligned(ptr, align);
}

void operator delete[](void* ptr, std::size_t size, std::align_val_t align) noexcept
{
    hf::utils::DeallocateAligned(ptr, align);
}

void operator delete(void* ptr, std::align_val_t align) noexcept
{
    hf::utils::DeallocateAligned(ptr, align);
}

void operator delete[](void* ptr, std::align_val_t align) noexcept
{
    hf::utils::DeallocateAligned(ptr, align);
}

void operator delete(void* ptr, std::align_val_t align, const std::nothrow_t&) noexcept
{
    hf::utils::DeallocateAligned(ptr, align);
}

void operator delete[](void* ptr, std::align_val_t align, const std::nothrow_t&) noexcept
{
    hf::utils::DeallocateAligned(ptr, align);
}

void operator delete(void* ptr, std::align_val_t align, std::size_t size) noexcept
{
    hf::utils::DeallocateAligned(ptr, align);
}

void operator delete[](void* ptr, std::align_val_t align, std::size_t size) noexcept
{
    hf::utils::DeallocateAligned(ptr, align);
}
