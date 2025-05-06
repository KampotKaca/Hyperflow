#include "hex_platform.h"
#include "hx11_platform.h"
#include "hinternal.h"

namespace hf::inter
{
    namespace platform
    {
        void Load()
        {
            X11_Load();
        }

        void Unload()
        {
            X11_Unload();
        }

        ivec2 GetPointerPosition(const Window* window)
        {
            return X11_GetPointerPosition(window);
        }

        void HandleEvents(EngineUpdateType updateType)
        {
            X11_HandleEvents(updateType);
        }

        void* GetPlatformInstance()
        {
            return PLATFORM_DATA.display;
        }
    }

    namespace window
    {
        void Open(Window* win)
        {
            X11_WindowOpen(win);
        }

        bool Close(Window* win)
        {
            if (win->handle)
            {
                rendering::DestroyRenderer_i(win->renderer.get());
                win->renderer = nullptr;
                X11_WindowClose(win);
                return true;
            }
            return false;
        }

        void SetTitle(const Window* win, const std::string& title)
        {
            X11_WindowSetTitle(win, title.c_str());
        }

        void SetSize(const Window* win, ivec2 size)
        {
            X11_WindowSetSize(win, size);
        }

        void SetPosition(const Window* win, ivec2 position)
        {
            X11_WindowSetPosition(win, position);
        }

        void SetRect(const Window* win, IRect rect)
        {
            X11_WindowSetRect(win, rect);
        }

        void SetFlags(Window* win, WindowFlags flags)
        {
            X11_WindowSetFlags(win, flags);
        }

        void Focus(const Window* win)
        {
            X11_WindowFocus(win);
        }

        void* GetWindowHandle(const Window* win)
        {
            return (void*)((X11Window*)win->handle)->window;
        }
    }
}
