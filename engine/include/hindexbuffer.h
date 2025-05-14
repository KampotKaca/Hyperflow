#ifndef HINDEXBUFFER_H
#define HINDEXBUFFER_H
#include "hshared.h"

namespace hf
{
    struct IndexBuffer
    {
        IndexBuffer(const IndexBufferCreationInfo& info, DataTransferType transferType);
        ~IndexBuffer();

        IndexBufferCreationInfo details{};
        DataTransferType transferType = DataTransferType::DoNotOwn;
        void* handle{};
    };
}

#endif //HINDEXBUFFER_H
