#ifndef HVERTEXBUFFER_H
#define HVERTEXBUFFER_H

#include "hshared.h"

namespace hf
{
    struct VertBuffer
    {
        VertBuffer(const VertBufferCreationInfo& info, DataTransferType transferType);
        ~VertBuffer();

        VertBufferCreationInfo details{};
        DataTransferType transferType = DataTransferType::DoNotOwn;
        void* handle{};
    };
}

#endif //HVERTEXBUFFER_H
