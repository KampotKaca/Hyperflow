#include "hvk_bufferallocator.h"
#include "hvk_graphics.h"

namespace hf
{
    VkBufferAllocator::VkBufferAllocator(const BufferAllocatorDefinitionInfo& info)
    {
        std::array<VkDescriptorPoolSize, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT + 1> poolSizes{};
        for (uint32_t i = 0; i < poolSizes.size(); i++)
        {
            poolSizes[i] =
            {
                .type = (VkDescriptorType)i,
                .descriptorCount = 0
            };
        }

        uint32_t totalDescriptorSets = 0;

        for (uint32_t i = 0; i < info.bufferCount; i++)
        {
            const auto& buffer = GetBuffer(info.pBuffers[i]);
            uint32_t descCount = 0;
            for (const auto& binding : buffer->bindings) descCount += binding.arraySize;
            uint32_t bufferCount = GetBufferCount(buffer->memoryType);
            descCount *= bufferCount;
            poolSizes[buffer->descriptorType].descriptorCount += descCount;

            totalDescriptorSets += bufferCount;
        }

        StaticVector<VkDescriptorPoolSize, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT + 1> chosenPoolSizes{};
        std::array<uint32_t, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT + 1> offsets{};

        {
            uint32_t currentSum = 0;
            for (auto& poolSize : poolSizes)
            {
                if (poolSize.descriptorCount > 0)
                {
                    chosenPoolSizes.push_back(poolSize);
                    if (chosenPoolSizes.size() >= 1)
                    {
                        const auto type = poolSize.type;
                        offsets[type] = currentSum;
                    }
                    currentSum += poolSize.descriptorCount;
                }
            }
        }

        const VkDescriptorPoolCreateInfo poolInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = totalDescriptorSets,
            .poolSizeCount = (uint32_t)chosenPoolSizes.size(),
            .pPoolSizes = chosenPoolSizes.begin(),
        };

        auto& device = GRAPHICS_DATA.device.logicalDevice.device;
        VK_HANDLE_EXCEPT(vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool));

        {
            uint32_t index = 0;
            for (uint32_t i = 0; i < info.bufferCount; i++)
            {
                const auto& buffer = GetBuffer(info.pBuffers[i]);
                uint32_t bufferCount = GetBufferCount(buffer->memoryType);
                for (uint32_t k = 0; k < bufferCount; k++)
                {
                    GRAPHICS_DATA.preAllocBuffers.descLayouts[index] = buffer->layout;
                    index++;
                }
            }
        }

        const VkDescriptorSetAllocateInfo allocInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = pool,
            .descriptorSetCount = totalDescriptorSets,
            .pSetLayouts = GRAPHICS_DATA.preAllocBuffers.descLayouts,
        };

        VK_HANDLE_EXCEPT(vkAllocateDescriptorSets(device, &allocInfo, GRAPHICS_DATA.preAllocBuffers.descriptors));

        {
            std::array<uint32_t, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT + 1> setIndices{};
            for (uint32_t i = 0; i < info.bufferCount; i++)
            {
                auto& buffer = GetBuffer(info.pBuffers[i]);
                const uint32_t bufferCount = GetBufferCount(buffer->memoryType);
                for (uint32_t k = 0; k < bufferCount; k++)
                {
                    const auto descType = buffer->descriptorType;
                    buffer->descriptorSets[k] = GRAPHICS_DATA.preAllocBuffers.descriptors[offsets[descType] + setIndices[descType]];
                    setIndices[descType]++;
                }
                SetupBuffer(buffer);
            }
        }
    }

    VkBufferAllocator::~VkBufferAllocator()
    {
        if (pool != VK_NULL_HANDLE)
            vkDestroyDescriptorPool(GRAPHICS_DATA.device.logicalDevice.device, pool, nullptr);
    }

    bool IsValidAllocator(BufferAllocator allocator)
    {
        return allocator > 0 && allocator <= GRAPHICS_DATA.bufferAllocators.size();
    }

    URef<VkBufferAllocator>& GetAllocator(BufferAllocator allocator)
    {
        if (!IsValidAllocator(allocator)) throw GENERIC_EXCEPT("[Hyperflow]", "Invalid uniform allocator");
        return GRAPHICS_DATA.bufferAllocators[allocator - 1];
    }
}