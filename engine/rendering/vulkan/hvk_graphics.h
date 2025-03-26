#ifndef HVK_GRAPHICS_H
#define HVK_GRAPHICS_H

#include "hshared.h"

namespace hf
{
    class VKGraphics
    {
    public:
        VKGraphics(void* windowHandle);
        ~VKGraphics();

        static void Init();
        static void Dispose();

    private:

        static int32_t s_RefCount;
    };
}

#endif //HVK_GRAPHICS_H
