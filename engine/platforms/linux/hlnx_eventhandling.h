#ifndef HLNX_EVENTHANDLING_H
#define HLNX_EVENTHANDLING_H

#include "hwindow.h"
#include <X11/Xlib.h>

namespace hf
{
    void Platform_HandleNextEvent();
    Window* Platform_GetWinPtr(::Window window);
    void Platform_PushWindowToRegistry(::Window window, Window* ptr);
    void Platform_PopWindowFromRegistry(::Window window);
}

#endif //HLNX_EVENTHANDLING_H
