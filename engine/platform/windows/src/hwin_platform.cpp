#include "hwin_shared.h"
#include "hplatform.h"

namespace hf::platform
{
    void Sleep(double seconds)
    {
        // A waitable timer seems to be better than the Windows Sleep().
        LARGE_INTEGER dueTime;
        seconds *= -10.0 * 1000.0 * 1000.0;
        dueTime.QuadPart = static_cast<LONGLONG>(seconds); //dueTime is in 100ns
        // We don't name the timer (third parameter) because CreateWaitableTimer will fail if the name
        // matches an existing name (e.g., if two threads call osaSleep).
        HANDLE waitTimer = CreateWaitableTimer(nullptr, true, nullptr);
        SetWaitableTimer(waitTimer, &dueTime, 0, nullptr, nullptr, 0);
        WaitForSingleObject(waitTimer, INFINITE);
        CloseHandle(waitTimer);
    }

    void* LoadDll(const char* dllName)
    {
        char p[MAX_PATH];
        GetModuleFileNameA(nullptr, p, MAX_PATH);
        std::filesystem::path exe(p);

        auto path = exe.parent_path() / (std::string("lib") + dllName + ".dll");
        if (!utils::FileExists(path.string().c_str()))
            throw GENERIC_EXCEPT("[Hyperflow]", "Unable to find dll at path %s", path.c_str());
        auto dll = LoadLibraryA(path.string().c_str());
        if (!dll) throw GENERIC_EXCEPT("[Hyperflow]", "Unable to load dll\n[Error] %s", GetLastError());
        return dll;
    }

    void UnloadDll(void* dll) { FreeLibrary((HMODULE)dll); }
    void* GetFuncPtr(void* dll, const char* funcName) { return (void*)GetProcAddress((HMODULE)dll, funcName); }
}