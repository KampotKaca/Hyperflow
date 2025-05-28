#include "hstoragebuffer.h"
#include "hinternal.h"
#include "hyperflow.h"

namespace hf
{
    StorageBuffer::StorageBuffer(const StorageBufferCreationInfo& info, DataTransferType transferType)
    : details(info), transferType(transferType)
    {
        if (transferType == DataTransferType::CopyData)
        {
            uint64_t bufferSize = info.elementCount * info.elementSizeInBytes;
            details.data = utils::Allocate(bufferSize);
            memcpy(details.data, info.data, bufferSize);
        }

        inter::rendering::CreateStorageBuffer_i(this);
    }

    StorageBuffer::~StorageBuffer()
    {
        if (transferType == DataTransferType::CopyData ||
            transferType == DataTransferType::TransferOwnership)
            utils::Deallocate(details.data);
        inter::rendering::DestroyStorageBuffer_i(this);
    }

    namespace storagebuffer
    {
        Ref<StorageBuffer> Create(const StorageBufferCreationInfo& info)
        {
            Ref<StorageBuffer> buffer = MakeRef<StorageBuffer>(info, DataTransferType::CopyData);
            inter::HF.graphicsResources.storageBuffers[(uint64_t)buffer.get()] = buffer;
            return buffer;
        }

        void Destroy(const Ref<StorageBuffer>& buffer)
        {
            if (inter::rendering::CreateStorageBuffer_i(buffer.get()))
                inter::HF.graphicsResources.storageBuffers.erase((uint64_t)buffer.get());
        }

        void Destroy(const Ref<StorageBuffer>* pBuffers, uint32_t count)
        {
            for (uint32_t i = 0; i < count; i++)
            {
                auto buffer = pBuffers[i];
                if (inter::rendering::DestroyStorageBuffer_i(buffer.get()))
                    inter::HF.graphicsResources.storageBuffers.erase((uint64_t)buffer.get());
            }
        }

        bool IsRunning(const Ref<StorageBuffer>& buffer) { return buffer->handle; }

        void Upload(const StorageBufferUploadInfo& info)
        {
            inter::rendering::StorageBufferUploadInfo uploadInfo
            {
                .storage = info.buffer->handle,
                .data = info.data,
                .offsetInBytes = info.offset * info.buffer->details.elementSizeInBytes,
                .sizeInBytes = info.size * info.buffer->details.elementSizeInBytes
            };
            inter::HF.renderingApi.api.UploadStorageBuffer(uploadInfo);
        }
    }

    namespace inter::rendering
    {
        bool CreateStorageBuffer_i(StorageBuffer* buffer)
        {
            if (buffer->handle) return false;
            buffer->handle = HF.renderingApi.api.CreateStorageBuffer(buffer->details);
            return true;
        }

        bool DestroyStorageBuffer_i(StorageBuffer* buffer)
        {
            if (buffer->handle)
            {
                HF.renderingApi.api.DestroyStorageBuffer(buffer->handle);
                buffer->handle = nullptr;
                return true;
            }
            return false;
        }

        void DestroyAllStorageBuffers_i(bool internalOnly)
        {
            for (const auto& buffer : std::ranges::views::values(HF.graphicsResources.storageBuffers))
                DestroyStorageBuffer_i(buffer.get());
            if (!internalOnly) HF.graphicsResources.storageBuffers.clear();
        }
    }
}