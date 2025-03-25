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
}

#endif //HLNX_WINDOW_H
