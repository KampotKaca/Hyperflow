#ifndef HVK_VERTBUFFER_H
#define HVK_VERTBUFFER_H

#include "hvk_shared.h"

namespace hf
{
    struct VkVertBuffer
    {
        explicit VkVertBuffer(const VertBufferCreationInfo& info);
        ~VkVertBuffer();

        uint32_t vertCount = 0;
        BufferAttrib attrib{};
        BufferMemoryType memoryType = BufferMemoryType::Static;

        VkBuffer buffer{};
        VmaAllocation bufferMemory{};

        void* mapping{};
    };
}

#endif //HVK_VERTBUFFER_H
