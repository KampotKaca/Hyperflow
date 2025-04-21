#ifndef HVERTEXBUFFER_H
#define HVERTEXBUFFER_H

#include "hshared.h"

namespace hf
{
    struct VertBuffer
    {
        VertBuffer(const VertBufferCreationInfo& info);
        ~VertBuffer();

        void* handle{};
    };
}

#endif //HVERTEXBUFFER_H
