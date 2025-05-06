#ifndef HVK_UNIFORMBUFFER_H
#define HVK_UNIFORMBUFFER_H

#include "hvk_shared.h"

namespace hf
{
    struct VkUniformBuffer
    {
        VkUniformBuffer(const UniformBufferDefinitionInfo& info);
        ~VkUniformBuffer();

        uint32_t bindingIndex{};
        uint32_t bufferSize{};
        std::vector<UniformBufferBindingInfo> bindings{};
        VkDescriptorSetLayout layout{};
        VkBuffer buffers[FRAMES_IN_FLIGHT]{};
        VmaAllocation memoryRegions[FRAMES_IN_FLIGHT]{};
        void* memoryMappings[FRAMES_IN_FLIGHT]{};
        VkDescriptorSet descriptorSets[FRAMES_IN_FLIGHT]{};
    };

    bool IsValidUniform(UniformBuffer buffer);
    const VkUniformBuffer& GetUniform(UniformBuffer buffer);
    void SetupUniform(UniformBuffer buffer, const VkDescriptorSet* pDescriptors);
}

#endif //HVK_UNIFORMBUFFER_H
