#ifndef HVK_BUFFERATTRIB_H
#define HVK_BUFFERATTRIB_H

#include "hvk_shared.h"

namespace hf
{
    struct VkBufferAttrib
    {
        VkBufferAttrib(const BufferAttribDefinitionInfo& info, uint32_t fullStride);
        ~VkBufferAttrib();

        VkVertexInputBindingDescription bindingDescription{};
        std::vector<VkVertexInputAttributeDescription> attribDescriptions{};
        uint32_t attribCount = 0;
        uint32_t vertexSize = 0;
        uint32_t bindingId = 0;
    };

    bool IsValidAttrib(BufferAttrib attrib);
    URef<VkBufferAttrib>& GetAttrib(BufferAttrib attrib);
}

#endif //HVK_BUFFERATTRIB_H
