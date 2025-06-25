#ifndef HBUFFER_H
#define HBUFFER_H

#include "hshared.h"

namespace hf
{
    enum class RuntimeBufferType { Vertex, Index };
    struct RuntimeBufferBase
    {
        virtual ~RuntimeBufferBase();
        [[nodiscard]] virtual RuntimeBufferType GetType() const = 0;

        DataTransferType transferType = DataTransferType::DoNotOwn;
        void* handle{};
        void* buffer{};
    };
}

#endif //HBUFFER_H
