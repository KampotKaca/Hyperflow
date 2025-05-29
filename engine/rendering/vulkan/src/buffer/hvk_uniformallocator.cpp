#include "hvk_uniformallocator.h"
#include "hvk_graphics.h"

namespace hf
{
    VkUniformAllocator::VkUniformAllocator(VkUniformAllocator&& other) noexcept
    {
        pool = other.pool;
        other.pool = VK_NULL_HANDLE;
    }

    VkUniformAllocator::VkUniformAllocator(const UniformAllocatorDefinitionInfo& info)
    {
        VkDescriptorPoolSize poolSize =
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 0
        };

        uint32_t totalBufferDescriptors = 0;
        uint32_t totalDescriptorSets = info.bufferCount * FRAMES_IN_FLIGHT;

        for (uint32_t i = 0; i < info.bufferCount; i++)
        {
            auto& buffer = GetUniform(info.pBuffers[i]);
            for (auto& binding : buffer.bindings)
                totalBufferDescriptors += binding.arraySize * FRAMES_IN_FLIGHT;
        }

        poolSize.descriptorCount = totalBufferDescriptors;

        VkDescriptorPoolCreateInfo poolInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = totalDescriptorSets,
            .poolSizeCount = 1,
            .pPoolSizes = &poolSize,
        };

        auto& device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        VK_HANDLE_EXCEPT(vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool));

        {
            uint32_t index = 0;
            for (uint32_t i = 0; i < info.bufferCount; i++)
            {
                auto& buffer = GetUniform(info.pBuffers[i]);
                for (uint32_t k = 0; k < FRAMES_IN_FLIGHT; k++)
                {
                    GRAPHICS_DATA.preAllocBuffers.descLayouts[index] = buffer.layout;
                    index++;
                }
            }
        }

        VkDescriptorSetAllocateInfo allocInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = pool,
            .descriptorSetCount = totalDescriptorSets,
            .pSetLayouts = GRAPHICS_DATA.preAllocBuffers.descLayouts,
        };

        VK_HANDLE_EXCEPT(vkAllocateDescriptorSets(device, &allocInfo, GRAPHICS_DATA.preAllocBuffers.descriptors));

        uint32_t setIndex = 0;
        for (uint32_t i = 0; i < info.bufferCount; i++)
        {
            auto& uniform = GetUniform(info.pBuffers[i]);
            for (auto& descriptorSet : uniform.descriptorSets)
            {
                descriptorSet = GRAPHICS_DATA.preAllocBuffers.descriptors[setIndex];
                setIndex++;
            }
            SetupUniform(uniform);
        }
    }

    VkUniformAllocator::~VkUniformAllocator()
    {
        if (pool != VK_NULL_HANDLE)
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