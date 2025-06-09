#ifndef HEX_PLATFORM_H
#define HEX_PLATFORM_H

#include "hshared.h"

namespace hf::inter
{
    namespace platform
    {
        void Load();
        void Unload();
        void Sleep(double seconds);

        ivec2 GetPointerPosition(const Window* window);
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
        struct Image
        {
            ivec2 size{};
            unsigned char* data{};
        };

        void Open(Window* win, const WindowCreationInfo& info);
        bool Close(Window* win);

        void SetTitle(Window* win, const std::string& title);
        void SetSize(const Window* win, ivec2 size);
        void SetPosition(const Window* win, ivec2 position);
        void SetRect(const Window* win, IRect rect);
        void SetPointerState(Window* win, WindowPointerState state);

        void SetState(Window* win, WindowState state);
        void SetEventFlags(Window* win, WindowEventFlags eventFlags);
        void Focus(const Window* win);

        bool IsFocused(const Window* win);

        IRect GetFrameRect(const Window* win);
        ivec2 GetSize(const Window* win);
        ivec2 GetPosition(const Window* win);

        void SetIcons(const Window* win, const Image* pImages, uint32_t count);
    }
}

#endif //HEX_PLATFORM_H
