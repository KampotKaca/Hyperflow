#ifndef HLNX_WINDOW_H
#define HLNX_WINDOW_H

#include <X11/Xutil.h>

namespace hf
{
    struct LnxWindowData
    {
        ::Window windowHandle;
        Atom closeMessage;
        Atom wmState;
        Atom maxHorz;
        Atom maxVert;
    };

    struct LnxPlatformHandle
    {
        Display* display;
        int32_t xiOpcode;
    };
}

#endif //HLNX_WINDOW_H
