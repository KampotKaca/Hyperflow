#include "hyperflow.h"
#include "hex_platform.h"
#include <dlfcn.h>
#include <bits/dlfcn.h>
#include <ctime>

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
}