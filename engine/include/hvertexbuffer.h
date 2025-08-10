#ifndef HVERTEXBUFFER_H
#define HVERTEXBUFFER_H

#include "hbuffer.h"
#include "hshared.h"

namespace hf
{
    struct VertexBuffer final : public RuntimeBufferBase
    {
        VertexBuffer(const VertexBufferCreationInfo& info, DataTransferType transferType);
        ~VertexBuffer() override = default;

        [[nodiscard]] RuntimeBufferType GetType() const override { return RuntimeBufferType::Vertex; }

        VertexBufferCreationInfo details{};
    };
}

#endif //HVERTEXBUFFER_H
