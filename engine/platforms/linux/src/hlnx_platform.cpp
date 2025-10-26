#include "hyperflow.h"
#include "hex_platform.h"
#include <dlfcn.h>
#include <bits/dlfcn.h>
#include <ctime>
#include "hyperflow.h"
#include "hplatformspecific.h"

#if defined(__clang__) || defined(__GNUC__)
    #define builtin_memcpy(destination, source, size) __builtin_memcpy(destination, source, size)
#elif defined(_MSC_VER)
    #define builtin_memcpy(destination, source, size) std::memcpy(destination, source, size)
#else
    #define builtin_memcpy(destination, source, size) std::memcpy(destination, source, size)
#endif

namespace hf::platform
{
    void* LoadDll(const char* dllName)
    {
        char exePath[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", exePath, PATH_MAX);
        std::string baseDir = std::string(exePath, (count > 0) ? count : 0);

        size_t lastSlash = baseDir.find_last_of('/');
        std::string exeDir = baseDir.substr(0, lastSlash + 1);

        std::string path = exeDir + "lib" + dllName + ".so";
        if (!utils::FileExists(path.c_str()))
            throw GENERIC_EXCEPT("[Hyperflow]", "Unable to find dll at path %s", path.c_str());

        void* dll = dlopen(path.c_str(), RTLD_NOW);
        if (!dll) throw GENERIC_EXCEPT("[Hyperflow]", "Unable to load dll\n[Error] %s", dlerror());
        return dll;
    }

    void* GetFuncPtr(void* dll, const char* funcName)
    {
        return dlsym(dll, funcName);
    }

    void UnloadDll(void* dll)
    {
        dlclose(dll);
    }

    void Sleep(double seconds)
    {
        timespec ts{};
        ts.tv_sec = (time_t)seconds;
        ts.tv_nsec = (long)((seconds - (double)ts.tv_sec) * 1e9);
        nanosleep(&ts, nullptr);
    }

    RenderingApiType GetBestRenderingApi()
    {
        return RenderingApiType::Vulkan;
    }

    bool IsValidRenderingApi(RenderingApiType api)
    {
        switch (api)
        {
            case RenderingApiType::Vulkan: return true;
            default: return false;
        }
    }

    size_t GetMemoryPageSize()
    {
        return static_cast<std::size_t>(sysconf(_SC_PAGESIZE));        // TYPICALLY 4096, 2^ 12
    }

    size_t GetMemoryPageAllocationGranularity() { return 4096; }

    static bool IsAddressAligned(void* address, std::size_t alignment)
    {
        auto address_in_question = reinterpret_cast<uint64_t>( address );
        auto remainder = address_in_question - (address_in_question / alignment) * alignment;
        return remainder == 0;
    }

    static bool IsPow2(std::size_t size)
    {
        return size > 0 && (size & (size - 1)) == 0;
    }

    // Page allocation granularity is always pow2
    static bool IsAddressPageAllocationGranularityAligned(void* address)
    {
        assert(IsPow2(GetMemoryPageAllocationGranularity()));
        return (reinterpret_cast<uint64_t>(address) & (GetMemoryPageAllocationGranularity() - 1)) == 0;
    }

        // Generic check including non pow2
    static bool IsSizeAMultipleOfPageAllocationGranularity(std::size_t input)
    {
        auto gran = GetMemoryPageAllocationGranularity();
        auto remainder = input - (input / gran) * gran;
        return remainder == 0;
    }

    static std::size_t GetNextPow2MultipleOf(std::size_t input, std::size_t multiple)
    {
        // Not checking if the given input is already a multiple
        return ((input + multiple - 1) & ~(multiple - 1));
    }

    int MallOpt(int param, int value) { return 0; }
    int AllocTrim(std::size_t pad) { return 0; }
    void* Valloc(std::size_t n) { return utils::AllocAligned(n, GetMemoryPageSize()); }
    void* Pvalloc(std::size_t n)
    {
        auto pageSize = platform::GetMemoryPageSize();
        return utils::AllocAligned(GetNextPow2MultipleOf(n, pageSize), pageSize);
    }
    void* ReallocArray(void *ptr, size_t n, size_t size)
    {
        unsigned long total = n * size;
        int err = __builtin_umull_overflow(n, size, &total);

        if (err)
        {
            errno = EINVAL;
            return nullptr;
        }

        return utils::Realloc(ptr, total);
    }

    int PosixMemAlign(void **memptr, std::size_t alignment, std::size_t size)
    {
        if(phmap::priv::internal_layout::adl_barrier::IsPow2(alignment) == false || memptr == nullptr) return EINVAL;

        if( size == 0 )
        {
            *memptr = nullptr;
            return 0;
        }

        auto ret = utils::AllocAligned(size, alignment);
        *memptr = ret;
        return (*memptr != nullptr) ? 0 : ENOMEM;
    }

    char* Strdup(const char *s)
    {
        std::size_t size = strlen(s);
        auto *new_str = static_cast<char*>(utils::Alloc(size + 1));

        if (new_str != nullptr)
        {
            builtin_memcpy(new_str, s, size);
            new_str[size] = '\0';
        }

        return new_str;
    }

    char* Strndup(const char *s, size_t n)
    {
        std::size_t size = strnlen(s, n);
        auto *new_str = static_cast<char*>(utils::Alloc(size+1));

        if (new_str != nullptr)
        {
            builtin_memcpy(new_str, s, size);
            new_str[size] = '\0';
        }

        return new_str;
    }

    char *RealPath(const char *path, char *resolved_path)
    {
        char *(*original_realpath)(const char *, char *) =(char *(*)(const char *, char *))dlsym(RTLD_NEXT, "realpath"); // Will invoke also our malloc

        if(original_realpath == nullptr) return nullptr;
        if(resolved_path != nullptr) return original_realpath(path, resolved_path);

        char* buffer = static_cast<char*>(utils::Alloc(PATH_MAX));
        return original_realpath(path, buffer);
    }
}