#include "hvertexbuffer.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "../../rendering/include/hex_renderer.h"

namespace hf
{
    VertexBuffer::VertexBuffer(const VertexBufferCreationInfo& info, DataTransferType transferType) : details(info)
    {
        if (info.vertexSize == 0) throw GENERIC_EXCEPT("[Hyperflow]", "vertex size must be greater than 0");

        this->transferType = transferType;
        if (transferType == DataTransferType::CopyData && info.pVertices)
        {
            const auto bufferSize = info.vertexCount * info.vertexSize;
            buffer = utils::Allocate(bufferSize);
            memcpy(buffer, info.pVertices, bufferSize);
        }

        inter::rendering::CreateVertBuffer_i(this);
    }

    Ref<VertexBuffer> Create(const VertexBufferCreationInfo& info)
    {
        Ref<VertexBuffer> buffer = MakeRef<VertexBuffer>(info, DataTransferType::CopyData);
        inter::HF.graphicsResources.buffers[(uint64_t)buffer.get()] = buffer;
        return buffer;
    }

    void Upload(const Ref<VertexBuffer>& vb, const VertBufferUploadInfo& info)
    {
        inter::rendering::VertexBufferUploadInfo_i uploadInfo{};
        uploadInfo.buffer = vb->handle;
        uploadInfo.data = vb->buffer;
        uploadInfo.offset = info.offset;
        uploadInfo.vertexCount = info.vertCount;

        inter::HF.renderingApi.api.UploadVertexBuffer(nullptr, uploadInfo);
    }

    namespace inter::rendering
    {
        bool CreateVertBuffer_i(VertexBuffer* buffer)
        {
            if (buffer->handle) return false;
            buffer->handle = HF.renderingApi.api.CreateVertexBuffer(buffer->details);
            return true;
        }
    }
}