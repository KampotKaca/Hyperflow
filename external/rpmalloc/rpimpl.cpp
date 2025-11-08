#include "hyperflow.h"
#include "rpmalloc.h"

namespace hf::utils
{
    void* Alloc(std::size_t n)
    {
        rpmalloc_initialize();
        void* memory = rpmalloc(n);
        if (!memory) throw std::bad_alloc();
        return memory;
    }

    void* AllocAligned(std::size_t n, std::size_t align)
    {
        rpmalloc_initialize();
        void* memory = rpaligned_alloc(align, n);
        if (!memory) throw std::bad_alloc();
        return memory;
    }

    void Deallocate(void* p) { rpfree(p); }
    void DeallocateAligned(void* p, std::size_t align) { rpfree(p); }
    void* Realloc(void* p, std::size_t n)
    {
        rpmalloc_initialize();
        return rprealloc(p, n);
    }
    void* ReallocAligned(void* p, std::size_t n, std::size_t align)
    {
        rpmalloc_initialize();
        return rpaligned_realloc(p, align, n, 0, 0);
    }
    void* Calloc(std::size_t n, std::size_t size)
    {
        rpmalloc_initialize();
        return rpcalloc(n, size);
    }

    std::size_t AllocUsableSize(void* ptr) { return rpmalloc_usable_size(ptr); }
    void CollectThreadMemoryCache() { rpmalloc_thread_collect(); }
}