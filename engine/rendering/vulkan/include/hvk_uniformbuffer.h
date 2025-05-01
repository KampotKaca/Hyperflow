#ifndef HVK_UNIFORMBUFFER_H
#define HVK_UNIFORMBUFFER_H

#include "hvk_shared.h"

namespace hf
{
    struct VkUniformBuffer
    {
        VkUniformBuffer(const UniformBufferDefinitionInfo& info);
        ~VkUniformBuffer();

        VkDescriptorSetLayout layout{};
    };
}

#endif //HVK_UNIFORMBUFFER_H
