#ifndef HLNX_WINDOW_H
#define HLNX_WINDOW_H

#include <X11/Xutil.h>

namespace hf
{
    struct LnxWindowData
    {
        ::Window window;
    };

    struct LnxPlatformData
    {
        Display* display;
        int32_t xiOpcode;

        Atom closeMessage;
        Atom wmState;
        Atom maxHorz;
        Atom maxVert;
    };

    extern LnxPlatformData PLATFORM_DATA;
    extern std::unordered_map<::Window, Window*> WIN_REGISTRY;
}

#endif //HLNX_WINDOW_H
