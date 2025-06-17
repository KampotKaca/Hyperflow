#ifndef HSTORAGEBUFFER_H
#define HSTORAGEBUFFER_H

#include "hbuffer.h"
#include "hshared.h"

namespace hf
{
    struct StorageBuffer : public Buffer
    {
        explicit StorageBuffer(const StorageBufferCreationInfo& info, DataTransferType transferType);
        ~StorageBuffer() override = default;

        [[nodiscard]] BufferType GetType() const override { return BufferType::Storage; }
        void Upload(const StorageBufferUploadInfo& info) const;
        static Ref<StorageBuffer> Create(const StorageBufferCreationInfo& info);

#ifndef HF_ENGINE_INTERNALS
    private:
#endif
        StorageBufferCreationInfo details{};
    };
}

#endif //HSTORAGEBUFFER_H
