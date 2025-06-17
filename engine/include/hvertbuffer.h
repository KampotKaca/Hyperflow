#ifndef HVERTEXBUFFER_H
#define HVERTEXBUFFER_H

#include "hbuffer.h"
#include "hshared.h"

namespace hf
{
    struct VertBuffer : public Buffer
    {
        VertBuffer(const VertBufferCreationInfo& info, DataTransferType transferType);
        ~VertBuffer() override = default;

        [[nodiscard]] BufferType GetType() const override { return BufferType::Vertex; }
        void Upload(const VertBufferUploadInfo& info) const;
        static Ref<VertBuffer> Create(const VertBufferCreationInfo& info);

#ifndef HF_ENGINE_INTERNALS
    private:
#endif
        VertBufferCreationInfo details{};
    };
}

#endif //HVERTEXBUFFER_H
