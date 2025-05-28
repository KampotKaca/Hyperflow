#include "hvertbuffer.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "../../rendering/include/hex_renderer.h"

namespace hf
{
    VertBuffer::VertBuffer(const VertBufferCreationInfo& info, DataTransferType transferType)
    : details(info), transferType(transferType)
    {
        if (info.bufferAttrib == 0) throw GENERIC_EXCEPT("[Hyperflow]", "buffer attribute must be set");

        if (transferType == DataTransferType::CopyData)
        {
            uint64_t bufferSize = info.vertexCount * inter::HF.renderingApi.api.GetVertBufferAttribSize(info.bufferAttrib);
            details.pVertices = utils::Allocate(bufferSize);
            memcpy(details.pVertices, info.pVertices, bufferSize);
        }

        inter::rendering::CreateVertBuffer_i(this);
    }

    VertBuffer::~VertBuffer()
    {
        if (transferType == DataTransferType::CopyData ||
            transferType == DataTransferType::TransferOwnership)
            utils::Deallocate(details.pVertices);
        inter::rendering::DestroyVertBuffer_i(this);
    }

    namespace vertbuffer
    {
        Ref<VertBuffer> Create(const VertBufferCreationInfo& info)
        {
            Ref<VertBuffer> buffer = MakeRef<VertBuffer>(info, DataTransferType::CopyData);
            inter::HF.graphicsResources.vertBuffers[(uint64_t)buffer.get()] = buffer;
            return buffer;
        }

        void Destroy(const Ref<VertBuffer>& buffer)
        {
            if (inter::rendering::DestroyVertBuffer_i(buffer.get()))
                inter::HF.graphicsResources.vertBuffers.erase((uint64_t)buffer.get());
        }

        void Destroy(const Ref<VertBuffer>* pBuffers, uint32_t count)
        {
            for (uint32_t i = 0; i < count; i++)
            {
                auto buffer = pBuffers[i];
                if (inter::rendering::DestroyVertBuffer_i(buffer.get()))
                    inter::HF.graphicsResources.vertBuffers.erase((uint64_t)buffer.get());
            }
        }

        bool IsRunning(const Ref<VertBuffer>& buffer) { return buffer->handle; }

        void Upload(const VertBufferUploadInfo& info)
        {
            inter::rendering::VertBufferUploadInfo uploadInfo
            {
                .buffer = info.buffer->handle,
                .data = info.data,
                .offset = info.offset,
                .vertexCount = info.vertCount
            };
            inter::HF.renderingApi.api.UploadVertBuffer(uploadInfo);
        }
    }

    namespace inter::rendering
    {
        bool CreateVertBuffer_i(VertBuffer* buffer)
        {
            if (buffer->handle) return false;
            buffer->handle = HF.renderingApi.api.CreateVertBuffer(buffer->details);
            return true;
        }

        bool DestroyVertBuffer_i(VertBuffer* buffer)
        {
            if (buffer->handle)
            {
                HF.renderingApi.api.DestroyVertBuffer(buffer->handle);
                buffer->handle = nullptr;
                return true;
            }
            return false;
        }

        void DestroyAllVertBuffers_i(bool internalOnly)
        {
            for (const auto& buffer : std::ranges::views::values(HF.graphicsResources.vertBuffers))
                DestroyVertBuffer_i(buffer.get());
            if (!internalOnly) HF.graphicsResources.vertBuffers.clear();
        }
    }

    namespace buffer
    {
        void SubmitAll()
        {
			inter::HF.renderingApi.api.SubmitBufferCopyOperations();
        }
    }
}