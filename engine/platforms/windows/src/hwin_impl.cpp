#include "hwin_shared.h"
#include "hwin_platform.h"
#include "hwin_window.h"
#include "hyperflow.h"

namespace hf::inter
{
    namespace platform
    {
        void Load()
        {
            Win_Load();
            Win_BeginTemporarySystemTimer(1);
        }

        void Unload()
        {
            Win_EndTemporarySystemTimer(1);
            Win_Unload();
        }

        void Sleep(double seconds)
        {
            Win_Sleep(seconds);
        }

        ivec2 GetPointerPosition(const Window* window)
        {
            return Win_GetPointerPosition(window);
        }

        void SetWindowFlag(WindowFlags* flags, WindowFlags target, uint32_t value)
        {
            Win_SetWindowFlag(flags, target, value);
        }

        void HandleEvents(EngineUpdateType updateType)
        {
            Win_HandleEvents(updateType);
        }

        void* LoadDll(const char* dllName)
        {
            const std::string path = std::string("lib") + dllName + ".dll";
            if (!utils::FileExists(path.c_str()))
                throw GENERIC_EXCEPT("[Hyperflow]", "Unable to find dll at path %s", path.c_str());
            return Win_LoadDll(path.c_str());
        }

        void UnloadDll(void* dll)
        {
            return Win_UnloadDll((HMODULE)dll);
        }

        void* GetFuncPtr(void* dll, const char* funcName)
        {
            return (void*)Win_GetFuncPtr(dll, funcName);
        }

        RenderingApiType GetBestRenderingApi()
        {
            return RenderingApiType::Direct3D;
        }

        bool IsValidRenderingApi(RenderingApiType api)
        {
            switch (api)
            {
                case RenderingApiType::Vulkan:
                case RenderingApiType::Direct3D: return true;
                default: return false;
            }
        }
    }

    namespace window
    {
        void Open(Window* win)
        {
            Win_WindowOpen(win);
        }

        bool Close(Window* win)
        {
            if (win->handle && IsWindow((HWND)win->handle))
            {
                rendering::DestroyRenderer_i(win->renderer.get());
                win->renderer = nullptr;
                Win_WindowClose(win);
                return true;
            }
            return false;
        }

        void SetTitle(const Window* win, const std::string& title)
        {
            Win_WindowSetTitle(win, title);
        }

        void SetSize(const Window* win, ivec2 size)
        {
            Win_WindowSetSize(win, size);
        }

        void SetPosition(const Window* win, ivec2 position)
        {
            Win_WindowSetPosition(win, position);
        }

        void SetRect(const Window* win, IRect rect)
        {
            Win_WindowSetRect(win, rect);
        }

        void SetFlags(Window* win, WindowFlags flags)
        {
            Win_WindowSetFlags(win, flags);
        }

        void Focus(const Window* win)
        {
            Win_WindowFocus(win);
        }
    }
}