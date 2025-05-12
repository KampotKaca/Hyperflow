#include "hvertbuffer.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "../../rendering/include/hex_renderer.h"

namespace hf
{
    VertBuffer::VertBuffer(const VertBufferCreationInfo& info)
    {
        if (info.bufferAttrib == 0) throw GENERIC_EXCEPT("[Hyperflow]", "buffer attribute must be set");
        uint64_t bufferSize = info.vertexCount * inter::HF.renderingApi.api.GetVertBufferAttribSize(info.bufferAttrib);
        creationInfo = info;
        creationInfo.pVertices = utils::Allocate(bufferSize);
        memcpy(creationInfo.pVertices, info.pVertices, bufferSize);

        inter::rendering::CreateVertBuffer_i(this);
    }

    VertBuffer::~VertBuffer()
    {
        utils::Deallocate(creationInfo.pVertices);
        inter::rendering::DestroyVertBuffer_i(this);
    }

    namespace vertbuffer
    {
        Ref<VertBuffer> Create(const VertBufferCreationInfo& info)
        {
            Ref<VertBuffer> buffer = MakeRef<VertBuffer>(info);
            inter::HF.graphicsResources.vertBuffers[buffer.get()] = buffer;
            return buffer;
        }

        void Destroy(const Ref<VertBuffer>& buffer)
        {
            if (inter::rendering::DestroyVertBuffer_i(buffer.get()))
                inter::HF.graphicsResources.vertBuffers.erase(buffer.get());
        }

        void Destroy(const Ref<VertBuffer>* pBuffers, uint32_t count)
        {
            for (uint32_t i = 0; i < count; i++)
            {
                auto buffer = pBuffers[i];
                if (inter::rendering::DestroyVertBuffer_i(buffer.get()))
                    inter::HF.graphicsResources.vertBuffers.erase(buffer.get());
            }
        }

        void DestroyAll(bool internalOnly)
        {
            for (const auto& buffer : std::ranges::views::values(inter::HF.graphicsResources.vertBuffers))
                inter::rendering::DestroyVertBuffer_i(buffer.get());
            if (!internalOnly) inter::HF.graphicsResources.vertBuffers.clear();
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
            buffer->handle = HF.renderingApi.api.CreateVertBuffer(buffer->creationInfo);
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
    }

    namespace buffer
    {
        void SubmitAll()
        {
			inter::HF.renderingApi.api.SubmitBufferCopyOperations();
        }
    }
}