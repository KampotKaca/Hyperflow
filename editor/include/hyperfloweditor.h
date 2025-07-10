#ifndef HHYPERFLOWEDITOR_H
#define HHYPERFLOWEDITOR_H

#include "hshared.h"

namespace hf::editor
{
    struct LoadInfo
    {
        void* windowHandle{};
        void* renderApiHandles{};
        MultisampleMode multisampleMode = MultisampleMode::MSAA_1X;
    };

    enum class WindowFlags
    {
        None = 0, NoTitleBar = 1 << 0, NoResize = 1 << 1, NoMove = 1 << 2, NoScrollbar = 1 << 3,
        NoScrollWithMouse = 1 << 4, NoCollapse = 1 << 5, AlwaysAutoResize = 1 << 6,
        NoBackground = 1 << 7, NoSavedSettings = 1 << 8, NoMouseInputs = 1 << 9,
        MenuBar  = 1 << 10, HorizontalScrollbar = 1 << 11, NoFocusOnAppearing = 1 << 12,
        NoBringToFrontOnFocus = 1 << 13, AlwaysVerticalScrollbar = 1 << 14,
        AlwaysHorizontalScrollbar = 1 << 15, NoNavInputs = 1 << 16,
        NoNavFocus = 1 << 17, UnsavedDocument = 1 << 18,
    };

    enum class Condition { None = 0, Always = 1 << 0, Once = 1 << 1, FirstUseEver = 1 << 2, Appearing = 1 << 3 };

#if DEBUG

    void Load(const LoadInfo& info);
    void Unload();

    void StartFrame();
    void EndFrame();
    void Draw(void* cmd);

    bool StartWindow(const char* name = "New Window", bool* isOpen = nullptr, WindowFlags flags = WindowFlags::None);
    void EndWindow();

    void SetNextWindowSize(vec2 size, Condition cond = Condition::None);
    void SetNextWindowPos(vec2 pos, Condition cond = Condition::None);

#else

    inline void Load(const LoadInfo& info){}
    inline void Unload(){}

    inline void StartFrame(){}
    inline void EndFrame(){}
    inline void Draw(void* cmd){}

    bool Start(const WindowInfo& info){}
    void End(){}

    void SetNextWindowSize(vec2 size, Condition cond = Condition::None){}
    void SetNextWindowPosition(vec2 pos, Condition cond = Condition::None){}

#endif
}

#endif //HHYPERFLOWEDITOR_H
