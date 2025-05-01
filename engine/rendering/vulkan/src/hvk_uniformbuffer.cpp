#include "hvk_uniformbuffer.h"

namespace hf
{
    VkUniformBuffer::VkUniformBuffer(const UniformBufferDefinitionInfo& info)
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding
        {
            .binding = info.bindingId,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = (uint32_t)info.usageStage,
            .pImmutableSamplers = nullptr,
        };
    }

    VkUniformBuffer::~VkUniformBuffer()
    {

    }
}