#ifndef HVK_VERTBUFFER_H
#define HVK_VERTBUFFER_H

#include "hvk_shared.h"
#include "hvk_buffer.h"

namespace hf
{
    struct VkVertexBuffer final : public VkBufferBase
    {
        explicit VkVertexBuffer(const inter::rendering::VertexBufferCreationInfo_i& info);
        ~VkVertexBuffer() override = default;
    };
}

#endif //HVK_VERTBUFFER_H
