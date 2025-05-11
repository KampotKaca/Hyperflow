#ifndef HVK_INDEXBUFFER_H
#define HVK_INDEXBUFFER_H

#include "hvk_shared.h"

namespace hf
{
    struct VkIndexBuffer
    {
        explicit VkIndexBuffer(const IndexBufferCreationInfo& info);
        ~VkIndexBuffer();

        VkBuffer buffer{};
        VmaAllocation bufferMemory{};
        uint32_t indexCount = 0;
        BufferMemoryType memoryType = BufferMemoryType::Static;
        BufferDataType indexFormat = BufferDataType::U16;
        VkIndexType indexType = VK_INDEX_TYPE_UINT16;
    };
}

#endif //HVK_INDEXBUFFER_H
