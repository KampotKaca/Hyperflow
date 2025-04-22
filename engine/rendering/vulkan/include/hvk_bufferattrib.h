#ifndef HVK_BUFFERATTRIB_H
#define HVK_BUFFERATTRIB_H

#include "hvk_shared.h"

namespace hf
{
    struct VkBufferAttrib
    {
        VkBufferAttrib(const BufferAttribCreateInfo& info, uint32_t fullStride);
        ~VkBufferAttrib();

        VkVertexInputBindingDescription bindingDescription{};
        std::vector<VkVertexInputAttributeDescription> attribDescriptions{};
        uint32_t attribCount = 0;
    };
}

#endif //HVK_BUFFERATTRIB_H
