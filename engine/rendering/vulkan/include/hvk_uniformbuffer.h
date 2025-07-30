#ifndef HVK_UNIFORMBUFFER_H
#define HVK_UNIFORMBUFFER_H

#include "hvk_boundbuffer.h"
#include "hvk_shared.h"

namespace hf
{
    struct VkUniformBuffer : public VkBoundBuffer
    {
        explicit VkUniformBuffer(const BufferDefinitionInfo& info);
        ~VkUniformBuffer() override = default;
    };
}

#endif //HVK_UNIFORMBUFFER_H
