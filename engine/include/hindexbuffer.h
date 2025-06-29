#ifndef HINDEXBUFFER_H
#define HINDEXBUFFER_H
#include "hbuffer.h"
#include "hshared.h"

namespace hf
{
    struct IndexBuffer : public RuntimeBufferBase
    {
        IndexBuffer(const IndexBufferCreationInfo& info, DataTransferType transferType);
        ~IndexBuffer() override = default;

        [[nodiscard]] RuntimeBufferType GetType() const override { return RuntimeBufferType::Index; }

        IndexBufferCreationInfo details{};
    };
}

#endif //HINDEXBUFFER_H
