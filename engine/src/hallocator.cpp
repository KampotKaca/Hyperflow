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

            auto& sp = *sts.span_use;
            LOG_LOG("\n[Span Use]\nCurrent: %0.2g\nPeak: %0.2g\nTo Global: %0.2g\nFrom Global: %0.2g\nTo Cache: %0.2g\nFrom Cache: %0.2g\nTo Reserved: %0.2g\nFrom Reserved: %0.2g\nMap Calls: %i",
                ToMB(sp.current), ToMB(sp.peak), ToMB(sp.to_global), ToMB(sp.from_global), ToMB(sp.to_cache), ToMB(sp.from_cache), ToMB(sp.to_reserved), ToMB(sp.from_reserved), sp.map_calls);

            auto& su = *sts.size_use;
            LOG_LOG("\n[Size Use]\nAlloc Current: %0.2g\nAlloc Peak: %0.2g\nAlloc Total: %0.2g\nFree Total: %0.2g\nSpans To Cache: %0.2g\nSpans From Cache: %0.2g\nSpans From Reserved: %0.2g\nMap Calls: %i",
                ToMB(su.alloc_current), ToMB(su.alloc_peak), ToMB(su.alloc_total), ToMB(su.free_total), ToMB(su.spans_to_cache), ToMB(su.spans_from_cache), ToMB(su.spans_from_reserved), su.map_calls);
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
