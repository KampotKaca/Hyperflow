#ifndef HFRAMEBUFFER_H
#define HFRAMEBUFFER_H

#include "hshared.h"

namespace hf
{
    struct FrameBuffer
    {
        FrameBuffer();
        ~FrameBuffer();

        void* handle{};
    };
}

#endif //HFRAMEBUFFER_H
