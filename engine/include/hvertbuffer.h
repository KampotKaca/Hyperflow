#ifndef HVERTEXBUFFER_H
#define HVERTEXBUFFER_H

#include "hshared.h"

namespace hf
{
    struct VertBuffer
    {
        VertBuffer(const VertBufferCreationInfo& info, bool storeDataLocally);
        ~VertBuffer();

        VertBufferCreationInfo details{};
        bool dataIsStoredLocally = false;
        void* handle{};
    };
}

#endif //HVERTEXBUFFER_H
