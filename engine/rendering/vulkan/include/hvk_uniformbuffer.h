#ifndef HVK_UNIFORMBUFFER_H
#define HVK_UNIFORMBUFFER_H

#include "hvk_buffer.h"
#include "hvk_shared.h"

namespace hf
{
    struct VkUniformBuffer : public VkBufferBase
    {
        explicit VkUniformBuffer(const BufferDefinitionInfo& info);
        ~VkUniformBuffer() override = default;
    };
}

#endif //HVK_UNIFORMBUFFER_H
