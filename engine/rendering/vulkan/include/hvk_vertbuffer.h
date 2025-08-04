#ifndef HVK_VERTBUFFER_H
#define HVK_VERTBUFFER_H

#include "hvk_shared.h"
#include "hvk_buffer.h"

namespace hf
{
    struct VkVertBuffer final : public VkBufferBase
    {
        explicit VkVertBuffer(const VertBufferCreationInfo& info);
        ~VkVertBuffer() override = default;

        uint32_t vertCount = 0;
        VertexBufferAttribute attrib{};
    };
}

#endif //HVK_VERTBUFFER_H
