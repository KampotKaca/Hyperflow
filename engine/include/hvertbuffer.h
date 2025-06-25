#ifndef HVERTEXBUFFER_H
#define HVERTEXBUFFER_H

#include "hbuffer.h"
#include "hshared.h"

namespace hf
{
    struct VertBuffer : public RuntimeBufferBase
    {
        VertBuffer(const VertBufferCreationInfo& info, DataTransferType transferType);
        ~VertBuffer() override = default;

        [[nodiscard]] RuntimeBufferType GetType() const override { return RuntimeBufferType::Vertex; }
        void Upload(const VertBufferUploadInfo& info) const;
        static Ref<VertBuffer> Create(const VertBufferCreationInfo& info);

        VertBufferCreationInfo details{};
    };
}

#endif //HVERTEXBUFFER_H
