#ifndef HVK_VERTBUFFER_H
#define HVK_VERTBUFFER_H

#include "hvk_shared.h"
#include "hvk_buffer.h"

namespace hf
{
    struct VkVertBuffer final : public VkBufferBase
    {
        explicit VkVertBuffer(const VertexBufferCreationInfo& info);
        ~VkVertBuffer() override = default;

        uint32_t vertCount = 0;
        uint32_t vertexSize = 0;
    };
}

#endif //HVK_VERTBUFFER_H
