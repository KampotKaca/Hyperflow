#ifndef HSTORAGEBUFFER_H
#define HSTORAGEBUFFER_H

#include "hshared.h"

namespace hf
{
    struct StorageBuffer
    {
        StorageBuffer(const StorageBufferCreationInfo& info);
        ~StorageBuffer();

        void* handle{};
    };
}

#endif //HSTORAGEBUFFER_H
