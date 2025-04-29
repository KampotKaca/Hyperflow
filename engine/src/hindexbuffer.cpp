#include "hindexbuffer.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "../rendering/include/hex_renderer.h"

namespace hf
{
    IndexBuffer::IndexBuffer(const IndexBufferCreationInfo& info)
    {
        if (info.indexCount % 3 != 0) throw GENERIC_EXCEPT("[Hyperflow]", "index count must be a multiple of 3");
        handle = inter::HF.renderingApi.api.CreateIndexBuffer(info);
    }

    IndexBuffer::~IndexBuffer()
    {
        inter::rendering::DestroyIndexBuffer_i(this);
    }

    namespace indexbuffer
    {
        Ref<IndexBuffer> Create(const IndexBufferCreationInfo& info)
        {
            Ref<IndexBuffer> buffer = MakeRef<IndexBuffer>(info);
            inter::HF.graphicsResources.indexBuffers[buffer.get()] = buffer;
            return buffer;
        }

        void Destroy(const Ref<IndexBuffer>& buffer)
        {
            if (inter::rendering::DestroyIndexBuffer_i(buffer.get()))
                inter::HF.graphicsResources.indexBuffers.erase(buffer.get());
        }

        void Destroy(const Ref<IndexBuffer>* pBuffers, uint32_t count)
        {
            for (uint32_t i = 0; i < count; i++)
            {
                auto buffer = pBuffers[i];
                if (inter::rendering::DestroyIndexBuffer_i(buffer.get()))
                    inter::HF.graphicsResources.indexBuffers.erase(buffer.get());
            }
        }

        void DestroyAll()
        {
            auto& indexBuffers = inter::HF.graphicsResources.indexBuffers;
            for (const auto& buffer : std::ranges::views::values(indexBuffers))
                inter::rendering::DestroyIndexBuffer_i(buffer.get());
            indexBuffers.clear();
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
        bool DestroyIndexBuffer_i(IndexBuffer* buffer)
        {
            if (buffer->handle)
            {
                HF.renderingApi.api.DestroyIndexBuffer(buffer->handle);
                buffer->handle = nullptr;
                return true;
            }
            return false;
        }
    }
}