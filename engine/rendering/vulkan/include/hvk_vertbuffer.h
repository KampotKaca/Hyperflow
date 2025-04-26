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
        VkDeviceMemory bufferMemory{};
        BufferAttrib attrib{};
        uint32_t vertCount = 0;
        void* data{};
        bool enableReadWrite = false;
    };
}

#endif //HVK_VERTBUFFER_H
