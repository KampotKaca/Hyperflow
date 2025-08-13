#include "hindexbuffer.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "../../rendering/include/hex_renderer.h"

namespace hf
{
    IndexBuffer::IndexBuffer(const IndexBufferCreationInfo& info, DataTransferType transferType) : details(info)
    {
        if (info.indexCount % 3 != 0) throw GENERIC_EXCEPT("[Hyperflow]", "index count must be a multiple of 3");

        this->transferType = transferType;
        if (transferType == DataTransferType::CopyData)
        {
            const auto bufferSize = info.indexCount * BUFFER_DATA_SIZE[(uint32_t)info.indexFormat];
            buffer = utils::Allocate(bufferSize);
            memcpy(buffer, info.pIndices, bufferSize);
        }

        inter::rendering::CreateIndexBuffer_i(this);
    }

    void Upload(const Ref<Renderer>& rn, const Ref<IndexBuffer>& ib, const IndexBufferUploadInfo& info)
    {
        inter::rendering::IndexBufferUploadInfo_i uploadInfo{};
        uploadInfo.buffer = ib->handle;
        uploadInfo.data = ib->buffer;
        uploadInfo.offset = info.offset;
        uploadInfo.indexCount = info.indexCount;

        void* rnHandle = nullptr;
        if (rn) rnHandle = rn->handle;
        inter::HF.renderingApi.api.UploadIndexBuffer(rnHandle, uploadInfo);
    }

    Ref<IndexBuffer> Create(const IndexBufferCreationInfo& info)
    {
        Ref<IndexBuffer> buffer = MakeRef<IndexBuffer>(info, DataTransferType::CopyData);
        inter::HF.graphicsResources.buffers[(uint64_t)buffer.get()] = buffer;
        return buffer;
    }

    namespace inter::rendering
    {
        bool CreateIndexBuffer_i(IndexBuffer* buffer)
        {
            if (buffer->handle) return false;
            buffer->handle = HF.renderingApi.api.CreateIndexBuffer(buffer->details);
            return true;
        }
    }
}