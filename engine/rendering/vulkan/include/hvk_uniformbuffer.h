#ifndef HVK_UNIFORMBUFFER_H
#define HVK_UNIFORMBUFFER_H

#include "hvk_shared.h"

namespace hf
{
    struct VkUniformBuffer
    {
        explicit VkUniformBuffer(const UniformBufferDefinitionInfo& info);
        ~VkUniformBuffer();

        uint32_t bindingIndex{};
        uint32_t bufferSize{};
        std::vector<UniformBufferBindingInfo> bindings{};
        VkDescriptorSetLayout layout{};
        VkBuffer buffers[FRAMES_IN_FLIGHT]{};
        VmaAllocation memoryRegions[FRAMES_IN_FLIGHT]{};
        void* memoryMappings[FRAMES_IN_FLIGHT]{};
        std::vector<VkDescriptorSet> descriptorSets{};
    };

    bool IsValidUniform(UniformBuffer buffer);
    const VkUniformBuffer& GetUniform(UniformBuffer buffer);
    uint32_t SetupUniform(UniformBuffer buffer, const VkDescriptorSet* pDescriptors);
}

#endif //HVK_UNIFORMBUFFER_H
