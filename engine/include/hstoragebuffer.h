#ifndef HSTORAGEBUFFER_H
#define HSTORAGEBUFFER_H

#include "hshared.h"

namespace hf
{
    struct StorageBuffer
    {
        explicit StorageBuffer(const StorageBufferCreationInfo& info, DataTransferType transferType);
        ~StorageBuffer();

        StorageBufferCreationInfo details{};
        DataTransferType transferType = DataTransferType::DoNotOwn;
        void* handle{};
    };
}

#endif //HSTORAGEBUFFER_H
