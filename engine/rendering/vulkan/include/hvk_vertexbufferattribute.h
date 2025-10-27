#ifndef HVK_BUFFERATTRIB_H
#define HVK_BUFFERATTRIB_H

#include "hvk_shared.h"

namespace hf
{
    struct VkVertexBufferAttribute
    {
        VkVertexBufferAttribute(const VertexBufferAttributeDefinitionInfo& info, uint32_t fullStride);
        ~VkVertexBufferAttribute();

        VkVertexInputBindingDescription bindingDescription{};
        SmallList<VkVertexInputAttributeDescription, RN_NUM_MAX_VERTEX_ATTRIBUTES> attribDescriptions{};
        uint32_t attribCount = 0;
        uint32_t vertexSize = 0;
        uint32_t bindingId = 0;
    };

    bool IsValidAttrib(VertexBufferAttribute attrib);
    URef<VkVertexBufferAttribute>& GetAttrib(VertexBufferAttribute attrib);
}

#endif //HVK_BUFFERATTRIB_H
