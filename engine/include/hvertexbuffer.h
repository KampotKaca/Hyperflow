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

        uint32_t vertexSize{};
        uint32_t vertexCount{};
        BufferMemoryType memoryType = BufferMemoryType::Static;
        BufferUsageTypeFlags usageFlags = BufferUsageTypeFlags::Vertex;
    };
}

#endif //HVERTEXBUFFER_H
