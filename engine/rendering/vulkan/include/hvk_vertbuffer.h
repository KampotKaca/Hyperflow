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
        VertBufferMemoryType memoryType = VertBufferMemoryType::Static;
    };

    void UploadVertBuffer(const VkVertBuffer* buffer, const void* data, uint32_t offset, uint32_t vertCount);
}

#endif //HVK_VERTBUFFER_H
