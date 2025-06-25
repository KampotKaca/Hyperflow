#include "hvertbuffer.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "../../rendering/include/hex_renderer.h"

namespace hf
{
    VertBuffer::VertBuffer(const VertBufferCreationInfo& info, DataTransferType transferType) : details(info)
    {
        if (info.bufferAttrib == 0) throw GENERIC_EXCEPT("[Hyperflow]", "buffer attribute must be set");

        this->transferType = transferType;
        if (transferType == DataTransferType::CopyData)
        {
            uint64_t bufferSize = info.vertexCount * inter::HF.renderingApi.api.GetVertBufferAttribSize(info.bufferAttrib);
            buffer = utils::Allocate(bufferSize);
            memcpy(buffer, info.pVertices, bufferSize);
        }

        inter::rendering::CreateVertBuffer_i(this);
    }

    Ref<VertBuffer> Create(const VertBufferCreationInfo& info)
    {
        Ref<VertBuffer> buffer = MakeRef<VertBuffer>(info, DataTransferType::CopyData);
        inter::HF.graphicsResources.buffers[(uint64_t)buffer.get()] = buffer;
        return buffer;
    }

    void Upload(const Ref<VertBuffer>& vb, const VertBufferUploadInfo& info)
    {
        const inter::rendering::VertBufferUploadInfo uploadInfo
        {
            .buffer = vb->handle,
            .data = vb->buffer,
            .offset = info.offset,
            .vertexCount = info.vertCount
        };
        inter::HF.renderingApi.api.UploadVertBuffer(uploadInfo);
    }

    namespace inter::rendering
    {
        bool CreateVertBuffer_i(VertBuffer* buffer)
        {
            if (buffer->handle) return false;
            buffer->handle = HF.renderingApi.api.CreateVertBuffer(buffer->details);
            return true;
        }
    }
}