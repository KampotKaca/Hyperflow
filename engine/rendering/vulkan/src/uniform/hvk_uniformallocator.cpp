#include "hvk_uniformallocator.h"
#include "hvk_graphics.h"

namespace hf
{
    VkUniformAllocator::VkUniformAllocator(const UniformAllocatorDefinitionInfo& info)
    {
        for (uint32_t i = (uint32_t)UniformBufferType::MinEnum; i < (uint32_t)UniformBufferType::MaxEnum; i++)
        {
            GRAPHICS_DATA.preAllocBuffers.descPoolSizes[i] =
            {
                .type = (VkDescriptorType)i,
                .descriptorCount = 0
            };
        }

        for (uint32_t i = 0; i < info.bufferCount; i++)
        {
            auto& buffer = GetUniform(info.pBuffers[i]);
            for (auto& binding : buffer.bindings)
                GRAPHICS_DATA.preAllocBuffers.descPoolSizes[(uint32_t)binding.type].descriptorCount += FRAMES_IN_FLIGHT;
        }

        uint32_t poolSize = 0;
        for (uint32_t i = (uint32_t)UniformBufferType::MinEnum; i < (uint32_t)UniformBufferType::MaxEnum; i++)
        {
            if (GRAPHICS_DATA.preAllocBuffers.descPoolSizes[i].descriptorCount > 0)
            {
                GRAPHICS_DATA.preAllocBuffers.descPoolSizes[poolSize] = GRAPHICS_DATA.preAllocBuffers.descPoolSizes[i];
                poolSize++;
            }
        }

        uint32_t descriptorCount = info.bufferCount * FRAMES_IN_FLIGHT;

        VkDescriptorPoolCreateInfo poolInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = descriptorCount,
            .poolSizeCount = poolSize,
            .pPoolSizes = GRAPHICS_DATA.preAllocBuffers.descPoolSizes,
        };

        auto& device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        VK_HANDLE_EXCEPT(vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool));

        for (uint32_t i = 0; i < info.bufferCount; i++)
        {
            auto& buffer = GetUniform(info.pBuffers[i]);
            for (uint32_t j = 0; j < FRAMES_IN_FLIGHT; j++)
                GRAPHICS_DATA.preAllocBuffers.descLayouts[i * FRAMES_IN_FLIGHT + j] = buffer.layout;
        }

        VkDescriptorSetAllocateInfo allocInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = pool,
            .descriptorSetCount = descriptorCount,
            .pSetLayouts = GRAPHICS_DATA.preAllocBuffers.descLayouts,
        };

        VK_HANDLE_EXCEPT(vkAllocateDescriptorSets(device, &allocInfo, GRAPHICS_DATA.preAllocBuffers.descriptors));
        for (uint32_t i = 0; i < info.bufferCount; i++)
            SetupUniform(info.pBuffers[i], &GRAPHICS_DATA.preAllocBuffers.descriptors[i * FRAMES_IN_FLIGHT]);
    }

    VkUniformAllocator::~VkUniformAllocator()
    {
        vkDestroyDescriptorPool(GRAPHICS_DATA.defaultDevice->logicalDevice.device, pool, nullptr);
    }

    bool IsValidAllocator(UniformAllocator allocator)
    {
        return allocator > 0 && allocator <= GRAPHICS_DATA.uniformAllocators.size();
    }

    const VkUniformAllocator& GetAllocator(UniformAllocator allocator)
    {
        if (!IsValidAllocator(allocator)) throw GENERIC_EXCEPT("[Hyperflow]", "Invalid uniform allocator");
        return GRAPHICS_DATA.uniformAllocators[allocator - 1];
    }
}