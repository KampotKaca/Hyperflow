#include "hvk_graphics.h"

namespace hf
{
    int32_t VKGraphics::s_RefCount = 0;

    VKGraphics::VKGraphics(void* windowHandle)
    {
        if (s_RefCount == 0) Init();

        s_RefCount++;
    }

    VKGraphics::~VKGraphics()
    {
        s_RefCount--;

        if (s_RefCount == 0) Dispose();
    }

}