#ifndef HVK_UNIFORMBUFFER_H
#define HVK_UNIFORMBUFFER_H

#include "hvk_shared.h"

namespace hf
{
    struct VkUniformBuffer
    {
        explicit VkUniformBuffer(const UniformBufferDefinitionInfo& info);
        ~VkUniformBuffer();
        VkUniformBuffer(VkUniformBuffer&& other) noexcept;

        bool isLoaded = false;
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
    VkUniformBuffer& GetUniform(UniformBuffer buffer);
    void SetupUniform(VkUniformBuffer& uniform);
}

#endif //HVK_UNIFORMBUFFER_H
