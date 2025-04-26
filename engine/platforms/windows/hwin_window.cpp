#include "hwin_shared.h"

#include "hyperflow.h"
#include "hinternal.h"
#include "hwindow.h"
#include "hplatform.h"
#include "exceptions/hwindowexception.h"

namespace hf::inter::window
{
    void Open(Window* win)
    {
        HWND parentHandle = nullptr;
        if (win->parent != nullptr) parentHandle = (HWND)win->parent->handle;

        ivec2 convertedSize = win->rect.size;
        Windows_ConvertSize(win, convertedSize);
        uint32_t currentStyle = Windows_GetStyleID(win->style);

        win->handle = CreateWindowEx
        (
            0,
            WINDOWS_CLASS_NAME,
            win->title.c_str(),
            currentStyle,
            win->rect.position[0], win->rect.position[1],
            convertedSize[0], convertedSize[1],
            parentHandle,
            nullptr,
            PLATFORM_DATA.instance,
            win
        );

        if (win->handle == nullptr) throw WND_LAST_EXCEPT();
    }

    bool Close(Window* win)
    {
        if (win->handle && IsWindow((HWND)win->handle))
        {
            rendering::DestroyRenderer(win->renderer.get());
            win->renderer = nullptr;
            if (!DestroyWindow((HWND)win->handle)) throw WND_LAST_EXCEPT();
            win->handle = nullptr;
            win->parent = nullptr;
            return true;
        }

        return false;
    }

    void SetTitle(const Window* win, const std::string& title)
    {
        if (!SetWindowText((HWND)win->handle, &title[0])) throw WND_LAST_EXCEPT();
    }

    void SetSize(const Window* win, ivec2 size)
    {
        Windows_ConvertSize(win, size);
        if (!SetWindowPos((HWND)win->handle, nullptr, 0, 0, size[0], size[1], SWP_NOMOVE))
            throw WND_LAST_EXCEPT();
    }

    void SetPosition(const Window* win, ivec2 position)
    {
        if (!SetWindowPos((HWND)win->handle, nullptr, position[0], position[1], 0, 0, SWP_NOSIZE))
            throw WND_LAST_EXCEPT();
    }

    void SetRect(const Window* win, IRect rect)
    {
        Windows_ConvertSize(win, rect.size);
        if (!SetWindowPos((HWND)win->handle, nullptr, rect.position[0], rect.position[1], rect.size[0], rect.size[1],
                          0))
            throw WND_LAST_EXCEPT();
    }

    void SetFlags(Window* win, WindowFlags flags)
    {
        if (win->flags == flags) return;

        if (((int32_t)win->flags & (int32_t)WindowFlags::Visible) != (
            (int32_t)flags & (int32_t)WindowFlags::Visible))
        {
            if ((int32_t)flags & (int32_t)WindowFlags::Visible) ShowWindow((HWND)win->handle, SW_SHOW);
            else ShowWindow((HWND)win->handle, SW_HIDE);
        }

        if (((int32_t)win->flags & (int32_t)WindowFlags::Minimized) != (
                (int32_t)flags & (int32_t)WindowFlags::Minimized) ||
            ((int32_t)win->flags & (int32_t)WindowFlags::Maximized) != (
                (int32_t)flags & (int32_t)WindowFlags::Maximized))
        {
            uint32_t both = (int32_t)flags & (int32_t)WindowFlags::Minimized + (int32_t)flags & (int32_t)
                WindowFlags::Maximized;
            if ((int32_t)flags & (int32_t)WindowFlags::Minimized) ShowWindow((HWND)win->handle, SW_MINIMIZE);
            if ((int32_t)flags & (int32_t)WindowFlags::Maximized) ShowWindow((HWND)win->handle, SW_MAXIMIZE);
            if (both == 0) ShowWindow((HWND)win->handle, SW_RESTORE);
        }

        win->flags = flags;
    }

    void Focus(const Window* win)
    {
        SetFocus((HWND)win->handle);
        SetForegroundWindow((HWND)win->handle);
    }
}

namespace hf::inter
{
    void* LoadDll(const char* dllName)
    {
        return LoadLibraryA(dllName);
    }

    void* GetFuncPtr(void* dll, const char* funcName)
    {
       return (void*)GetProcAddress((HMODULE)dll, funcName);
    }

    void UnloadDll(void* dll)
    {
        FreeLibrary((HMODULE)dll);
    }

    void* GetPlatformInstance()
    {
        return PLATFORM_DATA.instance;
    }
}
