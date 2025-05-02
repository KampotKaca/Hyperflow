#include "hvk_uniformallocator.h"
#include "hvk_graphics.h"

namespace hf
{
    VkUniformAllocator::VkUniformAllocator(const UniformAllocatorDefinitionInfo& info)
    {
        VkDescriptorPoolSize poolSize
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = FRAMES_IN_FLIGHT,
        };

        VkDescriptorPoolCreateInfo poolInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = FRAMES_IN_FLIGHT,
            .poolSizeCount = 1,
            .pPoolSizes = &poolSize,
        };

        VK_HANDLE_EXCEPT(vkCreateDescriptorPool(GRAPHICS_DATA.defaultDevice->logicalDevice.device, &poolInfo, nullptr, &pool));

        std::vector<VkDescriptorSetLayout> layouts(info.bufferCount * FRAMES_IN_FLIGHT);
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
        VK_HANDLE_EXCEPT(vkAllocateDescriptorSets(GRAPHICS_DATA.defaultDevice->logicalDevice.device, &allocInfo, descriptorSets.data()));
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