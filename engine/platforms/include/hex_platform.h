#ifndef HEX_PLATFORM_H
#define HEX_PLATFORM_H

#include "hshared.h"
#include "hkeyboard.h"
#include "hmouse.h"

namespace hf::inter
{
    namespace platform
    {
        void Load();
        void Unload();
        void Sleep(double seconds);

        ivec2 GetPointerPosition(const Window* window);
        void SetWindowFlag(WindowFlags* flags, WindowFlags target, uint32_t value);
        void HandleEvents(EngineUpdateType updateType);

        void* LoadDll(const char* dllName);
        void UnloadDll(void* dll);
        void* GetFuncPtr(void* dll, const char* funcName);

        RenderingApiType GetBestRenderingApi();
        bool IsValidRenderingApi(RenderingApiType api);
        uint32_t CreateVulkanSurface(void* windowHandle, void* instance, void* surfaceResult);
    }

    namespace window
    {
        void Open(Window* win);
        bool Close(Window* win);

        void SetTitle(const Window* win, const std::string& title);
        void SetSize(const Window* win, ivec2 size);
        void SetPosition(const Window* win, ivec2 position);
        void SetRect(const Window* win, IRect rect);

        void SetFlags(Window* win, WindowFlags flags);
        void SetEventFlags(Window* win, WindowEventFlags eventFlags);
        void Focus(const Window* win);
    }
}

#endif //HEX_PLATFORM_H
