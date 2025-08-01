#ifndef HVK_INDEXBUFFER_H
#define HVK_INDEXBUFFER_H

#include "hvk_shared.h"
#include "hvk_buffer.h"

namespace hf
{
    struct VkIndexBuffer : public VkBufferBase
    {
        explicit VkIndexBuffer(const IndexBufferCreationInfo& info);
        ~VkIndexBuffer() override = default;

        uint32_t indexCount = 0;
        BufferDataType indexFormat = BufferDataType::U16;
        VkIndexType indexType = VK_INDEX_TYPE_UINT16;
    };
}

#endif //HVK_INDEXBUFFER_H
