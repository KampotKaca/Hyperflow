#ifndef HVK_VERTBUFFER_H
#define HVK_VERTBUFFER_H

#include "hvk_shared.h"

namespace hf
{
    struct VkVertBuffer
    {
        VkVertBuffer(const VertBufferCreationInfo& info);
        ~VkVertBuffer();

        VkBuffer buffer{};
        VmaAllocation bufferMemory{};
        BufferAttrib attrib{};
        uint32_t vertCount = 0;
        BufferMemoryType memoryType = BufferMemoryType::Static;
    };
}

#endif //HVK_VERTBUFFER_H
