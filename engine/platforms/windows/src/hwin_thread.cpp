#include "hwin_shared.h"

using CreateThreadType = HANDLE(WINAPI*)(
    LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);

static CreateThreadType real_CreateThread = nullptr;

struct ThreadWrapper
{
    LPTHREAD_START_ROUTINE original_start;
    LPVOID original_arg;
};

DWORD WINAPI ThreadStartWrapper(LPVOID arg)
{
    hf::inter::alloc::LoadAllocatorThread();

    ThreadWrapper* wrapper = (ThreadWrapper*)arg;
    DWORD result = wrapper->original_start(wrapper->original_arg);

    delete wrapper;
    hf::inter::alloc::UnloadAllocatorThread();
    return result;
}

HANDLE WINAPI HookedCreateThread(
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    SIZE_T dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpThreadId
)
{
    auto* wrapper = new ThreadWrapper{ lpStartAddress, lpParameter };
    return real_CreateThread(lpThreadAttributes, dwStackSize, ThreadStartWrapper, wrapper, dwCreationFlags, lpThreadId);
}

// Simple IAT patching function
void HookIAT(const char* funcName, void* newFunc, void** originalFuncOut)
{
    HMODULE hModule = GetModuleHandle(nullptr); // Main executable
    ULONG size;
    PIMAGE_IMPORT_DESCRIPTOR importDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(
        hModule, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &size);

    for (; importDesc->Name; importDesc++) {
        LPCSTR modName = (LPCSTR)((PBYTE)hModule + importDesc->Name);
        HMODULE dll = GetModuleHandleA(modName);
        if (!dll) continue;

        PIMAGE_THUNK_DATA origThunk = (PIMAGE_THUNK_DATA)((PBYTE)hModule + importDesc->OriginalFirstThunk);
        PIMAGE_THUNK_DATA thunk = (PIMAGE_THUNK_DATA)((PBYTE)hModule + importDesc->FirstThunk);

        for (; origThunk->u1.AddressOfData; ++origThunk, ++thunk) {
            PIMAGE_IMPORT_BY_NAME import = (PIMAGE_IMPORT_BY_NAME)((PBYTE)hModule + origThunk->u1.AddressOfData);
            if (strcmp((char*)import->Name, funcName) == 0) {
                DWORD oldProtect;
                VirtualProtect(&thunk->u1.Function, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect);

                *originalFuncOut = (void*)thunk->u1.Function;
                thunk->u1.Function = (ULONG_PTR)newFunc;

                VirtualProtect(&thunk->u1.Function, sizeof(void*), oldProtect, &oldProtect);
                return;
            }
        }
    }
}

namespace hf
{
    void InitThreadHook()
    {
        HookIAT("CreateThread", (void*)HookedCreateThread, (void**)&real_CreateThread);
    }
}