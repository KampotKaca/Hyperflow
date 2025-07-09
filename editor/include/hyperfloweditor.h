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
        AlwaysHorizontalScrollbar = 1<< 15, NoNavInputs = 1 << 16,
        NoNavFocus = 1 << 17, UnsavedDocument = 1 << 18,
    };

#if DEBUG

    void Load(const LoadInfo& info);
    void Unload();

    void StartFrame();
    void EndFrame();
    void Draw(void* cmd);

    struct WindowCreationInfo
    {
        bool* pOpen{};
        WindowFlags flags = WindowFlags::None;
        std::optional<uvec2> size{};
        std::optional<uvec2> position{};
    };

    bool StartWindow(const char* name = "New Window", const WindowCreationInfo& info = {});
    void EndWindow();

#else

    inline void Load(const LoadInfo& info){}
    inline void Unload(){}

    inline void StartFrame(){}
    inline void EndFrame(){}
    inline void Draw(void* cmd){}

    bool StartWindow(const char* name = "New Window", const WindowCreationInfo& info = {}){}
    void EndWindow(){}

#endif
}

#endif //HHYPERFLOWEDITOR_H
