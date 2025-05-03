#ifndef HINDEXBUFFER_H
#define HINDEXBUFFER_H
#include "hshared.h"

namespace hf
{
    struct IndexBuffer
    {
        IndexBuffer(const IndexBufferCreationInfo& info);
        ~IndexBuffer();

        IndexBufferCreationInfo details{};
        void* handle{};
    };
}

#endif //HINDEXBUFFER_H
