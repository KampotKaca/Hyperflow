#include "hvk_uniformallocator.h"
#include "hvk_graphics.h"

namespace hf
{
    VkUniformAllocator::VkUniformAllocator(const UniformAllocatorDefinitionInfo& info)
    {
        std::unordered_map<UniformBufferType, VkDescriptorPoolSize> poolSizes{};

        for (uint32_t i = 0; i < info.bufferCount; i++)
        {
            auto& buffer = GetUniform(info.pBuffers[i]);

            for (auto& binding : buffer.bindings)
            {
                if (poolSizes.contains(binding.type))
                    poolSizes[binding.type].descriptorCount += FRAMES_IN_FLIGHT;
                else
                {
                    poolSizes[binding.type] =
                    {
                        .type = (VkDescriptorType)binding.type,
                        .descriptorCount = FRAMES_IN_FLIGHT,
                    };
                }
            }
        }

        std::vector<VkDescriptorPoolSize> pools(poolSizes.size());
        {
            uint32_t currentIndex = 0;
            for (auto& size : poolSizes | std::views::values)
            {
                pools[currentIndex] = size;
                currentIndex++;
            }
        }

        uint32_t descriptorCount = info.bufferCount * FRAMES_IN_FLIGHT;

        VkDescriptorPoolCreateInfo poolInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = descriptorCount,
            .poolSizeCount = (uint32_t)pools.size(),
            .pPoolSizes = pools.data(),
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