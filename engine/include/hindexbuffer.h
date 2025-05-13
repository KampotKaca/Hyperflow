#ifndef HINDEXBUFFER_H
#define HINDEXBUFFER_H
#include "hshared.h"

namespace hf
{
    struct IndexBuffer
    {
        IndexBuffer(const IndexBufferCreationInfo& info, bool storeDataLocally);
        ~IndexBuffer();

        IndexBufferCreationInfo details{};
        bool dataIsStoredLocally = false;
        void* handle{};
    };
}

#endif //HINDEXBUFFER_H
