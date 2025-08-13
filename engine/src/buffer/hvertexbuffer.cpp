#include "hvertexbuffer.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "../../rendering/include/hex_renderer.h"

namespace hf
{
    VertexBuffer::VertexBuffer(const VertexBufferCreationInfo& info, DataTransferType transferType) :
    vertexSize(info.vertexSize), vertexCount(info.vertexCount), memoryType(info.memoryType), usageFlags(info.usageFlags)
    {
        this->transferType = transferType;
        if (transferType == DataTransferType::CopyData && info.pVertices)
        {
            const auto bufferSize = info.vertexCount * info.vertexSize;
            buffer = utils::Allocate(bufferSize);
            memcpy(buffer, info.pVertices, bufferSize);
        }
        else buffer = info.pVertices;

        inter::rendering::CreateVertBuffer_i(this);
    }

    Ref<VertexBuffer> Create(const VertexBufferCreationInfo& info)
    {
        Ref<VertexBuffer> buffer = MakeRef<VertexBuffer>(info, DataTransferType::CopyData);
        inter::HF.graphicsResources.buffers[(uint64_t)buffer.get()] = buffer;
        return buffer;
    }

    void Upload(const Ref<Renderer>& rn, const Ref<VertexBuffer>& vb, const VertBufferUploadInfo& info)
    {
        inter::rendering::VertexBufferUploadInfo_i uploadInfo{};
        uploadInfo.buffer = vb->handle;
        uploadInfo.data = vb->buffer;
        uploadInfo.offsetInBytes = info.offset * vb->vertexSize;
        uploadInfo.sizeInBytes = info.vertCount * vb->vertexSize;

        void* rnHandle = nullptr;
        if (rn) rnHandle = rn->handle;
        inter::HF.renderingApi.api.UploadVertexBuffer(rnHandle, uploadInfo);
    }

    namespace inter::rendering
    {
        bool CreateVertBuffer_i(VertexBuffer* buffer)
        {
            if (buffer->handle) return false;

            VertexBufferCreationInfo_i info{};
            info.sizeInBytes = buffer->vertexCount * buffer->vertexSize;
            info.memoryType = buffer->memoryType;
            info.usageFlags = buffer->usageFlags;
            info.pVertices = buffer->buffer;

            buffer->handle = HF.renderingApi.api.CreateVertexBuffer(info);
            return true;
        }
    }
}