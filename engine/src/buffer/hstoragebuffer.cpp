#define HF_ENGINE_INTERNALS
#include "hstoragebuffer.h"
#include "hinternal.h"
#include "hyperflow.h"

namespace hf
{
    StorageBuffer::StorageBuffer(const StorageBufferCreationInfo& info, DataTransferType transferType) : details(info)
    {
        this->transferType = transferType;
        if (transferType == DataTransferType::CopyData)
        {
            uint64_t bufferSize = info.elementCount * info.elementSizeInBytes;
            buffer = utils::Allocate(bufferSize);
            if (info.data) memcpy(buffer, info.data, bufferSize);
        }

        inter::rendering::CreateStorageBuffer_i(this);
    }

    void StorageBuffer::Upload(const StorageBufferUploadInfo& info) const
    {
        inter::rendering::StorageBufferUploadInfo uploadInfo
        {
            .storage = handle,
            .data = buffer,
            .offsetInBytes = info.offset * details.elementSizeInBytes,
            .sizeInBytes = info.size * details.elementSizeInBytes
        };
        inter::HF.renderingApi.api.UploadStorageBuffer(uploadInfo);
    }

    Ref<StorageBuffer> StorageBuffer::Create(const StorageBufferCreationInfo& info)
    {
        Ref<StorageBuffer> buffer = MakeRef<StorageBuffer>(info, DataTransferType::CopyData);
        inter::HF.graphicsResources.buffers[(uint64_t)buffer.get()] = buffer;
        return buffer;
    }

    namespace inter::rendering
    {
        bool CreateStorageBuffer_i(StorageBuffer* buffer)
        {
            if (buffer->handle) return false;
            buffer->handle = HF.renderingApi.api.CreateStorageBuffer(buffer->details);
            return true;
        }
    }
}