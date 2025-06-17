#ifndef HINDEXBUFFER_H
#define HINDEXBUFFER_H
#include "hbuffer.h"
#include "hshared.h"

namespace hf
{
    struct IndexBuffer : public Buffer
    {
        IndexBuffer(const IndexBufferCreationInfo& info, DataTransferType transferType);
        ~IndexBuffer() override = default;

        [[nodiscard]] BufferType GetType() const override { return BufferType::Index; }
        void Upload(const IndexBufferUploadInfo& info) const;
        static Ref<IndexBuffer> Create(const IndexBufferCreationInfo& info);

#ifndef HF_ENGINE_INTERNALS
    private:
#endif
        IndexBufferCreationInfo details{};
    };
}

#endif //HINDEXBUFFER_H
