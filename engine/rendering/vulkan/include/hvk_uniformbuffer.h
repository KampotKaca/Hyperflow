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
        VkBuffer buffers[FRAMES_IN_FLIGHT]{};
        VmaAllocation memoryRegions[FRAMES_IN_FLIGHT]{};
        void* memoryMappings[FRAMES_IN_FLIGHT]{};
    };

    bool IsValidUniform(UniformBuffer buffer);
    const VkUniformBuffer& GetUniform(UniformBuffer buffer);
}

#endif //HVK_UNIFORMBUFFER_H
