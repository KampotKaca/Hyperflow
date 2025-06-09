#include "hindexbuffer.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "../../rendering/include/hex_renderer.h"

namespace hf
{
    IndexBuffer::IndexBuffer(const IndexBufferCreationInfo& info, DataTransferType transferType)
    : details(info), transferType(transferType)
    {
        if (info.indexCount % 3 != 0) throw GENERIC_EXCEPT("[Hyperflow]", "index count must be a multiple of 3");

        if (transferType == DataTransferType::CopyData)
        {
            uint64_t bufferSize = info.indexCount * BUFFER_DATA_SIZE[(uint32_t)info.indexFormat];
            details.pIndices = utils::Allocate(bufferSize);
            memcpy(details.pIndices, info.pIndices, bufferSize);
        }

        inter::rendering::CreateIndexBuffer_i(this);
    }

    IndexBuffer::~IndexBuffer()
    {
        if (transferType == DataTransferType::CopyData ||
            transferType == DataTransferType::TransferOwnership)
            utils::Deallocate(details.pIndices);
        inter::rendering::DestroyIndexBuffer_i(this);
    }

    namespace indexbuffer
    {
        Ref<IndexBuffer> Create(const IndexBufferCreationInfo& info)
        {
            Ref<IndexBuffer> buffer = MakeRef<IndexBuffer>(info, DataTransferType::CopyData);
            inter::HF.graphicsResources.indexBuffers[(uint64_t)buffer.get()] = buffer;
            return buffer;
        }

        void Destroy(const Ref<IndexBuffer>& buffer)
        {
            if (inter::rendering::DestroyIndexBuffer_i(buffer.get()))
                inter::HF.graphicsResources.indexBuffers.erase((uint64_t)buffer.get());
        }

        void Destroy(const Ref<IndexBuffer>* pBuffers, uint32_t count)
        {
            for (uint32_t i = 0; i < count; i++)
            {
                auto buffer = pBuffers[i];
                if (inter::rendering::DestroyIndexBuffer_i(buffer.get()))
                    inter::HF.graphicsResources.indexBuffers.erase((uint64_t)buffer.get());
            }
        }

        bool IsRunning(const Ref<IndexBuffer>& buffer) { return buffer->handle; }

        void Upload(const IndexBufferUploadInfo& info)
        {
            inter::rendering::IndexBufferUploadInfo uploadInfo
            {
                .buffer = info.buffer->handle,
                .data = info.data,
                .offset = info.offset,
                .indexCount = info.indexCount
            };
            inter::HF.renderingApi.api.UploadIndexBuffer(uploadInfo);
        }
    }

    namespace inter::rendering
    {
        bool CreateIndexBuffer_i(IndexBuffer* buffer)
        {
            if (buffer->handle) return false;
            buffer->handle = HF.renderingApi.api.CreateIndexBuffer(buffer->details);
            return true;
        }

        bool DestroyIndexBuffer_i(IndexBuffer* buffer)
        {
            if (buffer->handle)
            {
                std::lock_guard lock(HF.deletedResources.syncLock);
                HF.deletedResources.indexBuffers.push_back(buffer->handle);
                buffer->handle = nullptr;
                return true;
            }
            return false;
        }

        void DestroyAllIndexBuffers_i(bool internalOnly)
        {
            for (const auto& buffer : std::ranges::views::values(HF.graphicsResources.indexBuffers))
                DestroyIndexBuffer_i(buffer.get());
            if (!internalOnly) HF.graphicsResources.indexBuffers.clear();
        }
    }
}