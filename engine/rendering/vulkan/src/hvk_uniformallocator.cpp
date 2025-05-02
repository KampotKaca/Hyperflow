#include "hvk_uniformallocator.h"
#include "hvk_graphics.h"

namespace hf
{
    VkUniformAllocator::VkUniformAllocator(const UniformAllocatorDefinitionInfo& info)
    {
        uint32_t descriptorCount = info.bufferCount * FRAMES_IN_FLIGHT;
        VkDescriptorPoolSize poolSize
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = descriptorCount,
        };

        VkDescriptorPoolCreateInfo poolInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = descriptorCount,
            .poolSizeCount = 1,
            .pPoolSizes = &poolSize,
        };

        auto& device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        VK_HANDLE_EXCEPT(vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool));

        std::vector<VkDescriptorSetLayout> layouts(descriptorCount);
        for (uint32_t i = 0; i < info.bufferCount; i++)
        {
            auto& buffer = GetUniform(info.pBuffers[i]);
            for (uint32_t j = 0; j < FRAMES_IN_FLIGHT; j++)
                layouts[i * FRAMES_IN_FLIGHT + j] = buffer.layout;
        }

        VkDescriptorSetAllocateInfo allocInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = pool,
            .descriptorSetCount = (uint32_t)layouts.size(),
            .pSetLayouts = layouts.data(),
        };

        std::vector<VkDescriptorSet> descriptorSets(layouts.size());
        VK_HANDLE_EXCEPT(vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()));
        for (uint32_t i = 0; i < info.bufferCount; i++)
            SetupUniform(info.pBuffers[i], &descriptorSets[i * FRAMES_IN_FLIGHT]);
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