#ifndef HYPERFLOW_HPLATFORMSPECIFIC_H
#define HYPERFLOW_HPLATFORMSPECIFIC_H

#include "../../../../config.h"
#include <cstddef>

#if defined(HF_ENABLE_ALLOCATOR)

#ifdef _WIN32
#define _aligned_malloc(size, alignment) hf::utils::AllocAligned(size, alignment)
#define _aligned_free(ptr) hf::utils::DeallocateAligned(ptr)
#define _msize(ptr) hf::utils::AllocUsableSize(ptr)
#define _aligned_realloc(ptr, size, alignment) hf::utils::ReallocAlligned(ptr, size, alignment)
#endif

#ifdef __linux__
struct mallinfo
{
    int arena;    // total space allocated from the system
    int ordblks;  // number of non-inuse chunks
    int smblks;   // unused -- always zero
    int hblks;    // number of mmapped regions
    int hblkhd;   // total space in mmapped regions
    int usmblks;  // unused -- always zero
    int fsmblks;  // unused -- always zero
    int uordblks; // total allocated space
    int fordblks; // total non-inuse space
    int keepcost; // top-most, releasable space (in bytes)
};

namespace hf::platform
{
    int MallOpt(int param, int value);
    int AllocTrim(std::size_t pad);
    void* Valloc(std::size_t n);
    void* Pvalloc(std::size_t n);
    void* ReallocArray(void *ptr, size_t n, size_t size);

    int PosixMemAlign(void **memptr, std::size_t alignment, std::size_t size);

    char* Strdup(const char *s);
    char* Strndup(const char *s, size_t n);
    char *RealPath(const char *path, char *resolved_path);
}

#define aligned_alloc(alignment, size) hf::utils::AllocAligned(size, alignment)
#define malloc_usable_size(ptr) hf::utils::AllocUsableSize(ptr)
#define cfree(ptr) hf::utils::Deallocate(ptr)
#define memalign(alignment, size) hf::utils::AllocAligned(size, alignment)
#define reallocf(ptr, size) hf::utils::Realloc(ptr, size)
#define reallocarray(ptr, nelem, elsize) hf::platform::ReallocArray(ptr, nelem, elsize)
#define mallinfo struct mallinfo info = {0}
#define mallopt(param, value) hf::platform::MallOpt(param, value)
#define malloc_trim(pad) hf::platform::AllocTrim(pad)
#define valloc(size) hf::platform::Valloc(size)
#define pvalloc(size) hf::platform::Pvalloc(size)
#define posix_memalign(memptr, alignment, size) hf::platform::PosixMemAlign(memptr, alignment, size)
#define strdup(s) hf::platform::Strdup(s)
#define strndup(s, n) hf::platform::Strndup(s, n)
#define realpath(path, resolved_path) hf::platform::RealPath(path, resolved_path)
#endif

#endif

#endif //HYPERFLOW_HPLATFORMSPECIFIC_H
