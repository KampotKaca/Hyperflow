#include "rpmalloc.h"
#include "hutilfuncs.h"

namespace hf::utils
{
    void* Alloc(std::size_t n)
    {
        rpmalloc_initialize();
        const auto memory = rpmalloc(n);
        hassert(memory, "Failed to allocate memory");
        return memory;
    }

    void* AllocAligned(std::size_t n, std::size_t align)
    {
        rpmalloc_initialize();
        const auto memory = rpaligned_alloc(align, n);
        hassert(memory, "Failed to allocate aligned memory");
        return memory;
    }

    void Free(void* p) { rpfree(p); }
    void* Realloc(void* p, std::size_t n)
    {
        rpmalloc_initialize();
        const auto mem = rprealloc(p, n);
        hassert(mem, "Failed to reallocate memory");
        return mem;
    }
    void* ReallocAligned(void* p, std::size_t n, std::size_t align)
    {
        rpmalloc_initialize();
        const auto mem = rpaligned_realloc(p, align, n, 0, 0);
        hassert(mem, "Failed to reallocate aligned memory");
        return mem;
    }
    void* Calloc(std::size_t n, std::size_t size)
    {
        rpmalloc_initialize();
        const auto mem =  rpcalloc(n, size);
        hassert(mem, "Failed to callocate memory");
        return mem;
    }

    std::size_t AllocUsableSize(void* ptr) { return rpmalloc_usable_size(ptr); }
    void CollectThreadMemoryCache() { rpmalloc_thread_collect(); }
}