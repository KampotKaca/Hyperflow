#include "hvk_indexbuffer.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    static uint64_t GetIndexBufferSize(BufferDataType indexFormat, uint32_t indexCount)
    {
        return (uint64_t)BUFFER_DATA_SIZE[(uint32_t)indexFormat] * indexCount;
    }

    VkIndexBuffer::VkIndexBuffer(const IndexBufferCreationInfo& info)
        : VkBufferBase(info.memoryType, (uint8_t*)info.pIndices, 0, 0,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | (uint32_t)info.usageFlags,
            GetIndexBufferSize(info.indexFormat, info.indexCount)),
        indexCount(info.indexCount), indexFormat(info.indexFormat)
    {
        switch (indexFormat)
        {
            case BufferDataType::U8:  indexType = VK_INDEX_TYPE_UINT8;  break;
            case BufferDataType::U16: indexType = VK_INDEX_TYPE_UINT16; break;
            case BufferDataType::U32: indexType = VK_INDEX_TYPE_UINT32; break;
            default:
            log_error_s("[Hyperflow] Invalid index format!!!");
            indexType = VK_INDEX_TYPE_UINT8;
            break;
        }
    }

    void UploadBuffer(const VkRenderer* rn, const VkIndexBuffer* buffer, const void* data,
        uint32_t offset, uint32_t indexCount)
    {
        uint32_t currentFrame = 0;
        if (rn) currentFrame = rn->currentFrame;

        hassert(buffer->memoryType != BufferMemoryType::Static, "[Hyperflow] Cannot modify static buffer")

        const auto fullSize = (uint64_t)BUFFER_DATA_SIZE[(uint32_t)buffer->indexFormat] * indexCount;
        const auto fullOffset = (uint64_t)offset * indexCount;

        memcpy((uint8_t*)buffer->memoryMappings[currentFrame] + fullOffset, data, fullSize);
    }
}