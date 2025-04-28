#ifndef HEX_PLATFORM_H
#define HEX_PLATFORM_H

#include "hshared.h"
#include "hkeyboard.h"
#include "hmouse.h"

namespace hf::inter
{
    struct PlatformCallbacks
    {
        void (*KeyboardEvent_Key)(Keyboard& keyboard, Key key, Keyboard::Event::Type type) noexcept;
        void (*KeyboardEvent_Char)(Keyboard& keyboard, char character) noexcept;

        void (*MouseEvent_Button)(Mouse& mouse, Button button, Mouse::Event::Type type) noexcept;
        void (*MouseEvent_Moved)(Mouse& mouse, ivec2 position) noexcept;
        void (*MouseEvent_Scroll)(Mouse& mouse, vec2 delta) noexcept;

        void (*WindowEvent_Title)(Window* win, const char* newTitle) noexcept;
        void (*WindowEvent_Close)(Window* win) noexcept;
        void (*WindowEvent_Show)(Window* win, bool show) noexcept;
        void (*WindowEvent_Move)(Window* win, ivec2 position) noexcept;
        void (*WindowEvent_Resize)(Window* win, ivec2 size) noexcept;
        void (*WindowEvent_Focus)(Window* win, bool isFocused) noexcept;
    };

    namespace platform
    {
        void Load(const PlatformCallbacks& callbacks);
        void Unload();
        void Sleep(double seconds);

        ivec2 GetPointerPosition(const Window* window);
        void SetWindowFlag(WindowFlags* flags, WindowFlags target, uint32_t value);
        void HandleEvents(EngineUpdateType updateType);

        void* LoadDll(const char* dllName);
        void UnloadDll(void* dll);
        void* GetFuncPtr(void* dll, const char* funcName);
        void* GetPlatformInstance();
    }

    namespace window
    {
        void Open(Window* win);
        bool Close(Window* win);

        void SetTitle(const Window* win, const std::string& title);
        void SetSize(const Window* win, ivec2 size);
        void SetPosition(const Window* win, ivec2 position);
        void SetRect(const Window* win, IRect rect);

        void* GetWindowHandle(const Window* win);

        void SetFlags(Window* win, WindowFlags flags);
        void Focus(const Window* win);
        void Update(const Window* win);
    }
}

#endif //HEX_PLATFORM_H
