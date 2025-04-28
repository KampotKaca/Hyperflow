#ifndef HWIN_WINDOW_H
#define HWIN_WINDOW_H

#include "hwin_shared.h"

namespace hf
{
    void Win_WindowOpen(Window* win);
    void Win_WindowClose(Window* win);

    void Win_WindowSetTitle(const Window* win, const std::string& title);
    void Win_WindowSetSize(const Window* win, ivec2 size);
    void Win_WindowSetPosition(const Window* win, ivec2 position);
    void Win_WindowSetRect(const Window* win, IRect rect);

    void Win_WindowSetFlags(Window* win, WindowFlags flags);
    void Win_WindowFocus(const Window* win);
    void* Win_GetWindowHandle(const Window* win);
}

#endif //HWIN_WINDOW_H
