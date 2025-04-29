#include "hvk_graphics.h"

namespace hf
{
    static constexpr uint32_t VK_MEMORY_TYPE[(uint32_t)BufferMemoryType::Count]
    {
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };

    uint32_t GetMemoryType(uint32_t filter, VkMemoryPropertyFlags props)
    {
        auto& memProps = GRAPHICS_DATA.defaultDevice->memProps;
        for (uint32_t i = 0; i < memProps.memoryTypeCount; i++)
        {
            if ((filter & (1 << i)) && (memProps.memoryTypes[i].propertyFlags & props) == props) return i;
        }

        throw GENERIC_EXCEPT("[Hyperflow]", "Unable to allocate graphics memory");
    }

    void CreateBuffer(const VkCreateBufferInfo& info, VkBuffer* bufferResult, VkDeviceMemory* memResult)
    {
        VkBufferCreateInfo bufferInfo
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = info.size,
            .usage = info.usage,
            .sharingMode = info.sharingMode,
            .queueFamilyIndexCount = info.familyCount,
            .pQueueFamilyIndices = info.pQueueFamilies
        };

        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        VK_HANDLE_EXCEPT(vkCreateBuffer(device, &bufferInfo, nullptr, bufferResult));
        VkMemoryRequirements memReqs{};
        vkGetBufferMemoryRequirements(device, *bufferResult, &memReqs);

        VkMemoryAllocateInfo allocInfo
        {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memReqs.size,
            .memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_TYPE[(uint32_t)info.memoryType])
        };

        VK_HANDLE_EXCEPT(vkAllocateMemory(device, &allocInfo, nullptr, memResult));
        VK_HANDLE_EXCEPT(vkBindBufferMemory(device, *bufferResult, *memResult, 0));
    }

    void StageCopyOperation(const VkCopyBufferOperation& operation)
    {
        GRAPHICS_DATA.bufferCopyOperations.push_back(operation);
    }

    void SubmitStagedCopyOperations()
    {
        if (!GRAPHICS_DATA.bufferCopyOperations.empty())
        {
            CopyBufferContents(GRAPHICS_DATA.bufferCopyOperations.data(),
                         GRAPHICS_DATA.bufferCopyOperations.size());
            GRAPHICS_DATA.bufferCopyOperations.clear();
        }
    }

    void UploadBufferMemory(VkDeviceMemory memory, const void* data, uint64_t fullOffset, uint64_t fullSize)
    {
        void* mapping;
        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        VK_HANDLE_EXCEPT(vkMapMemory(device, memory, fullOffset, fullSize, 0, &mapping));
        memcpy(mapping, data, fullSize);
        vkUnmapMemory(device, memory);
    }

    void CopyBufferContents(const VkCopyBufferOperation* pOperations, uint32_t operationCount)
    {
        VkCommandBufferBeginInfo beginInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        };

        auto command = GRAPHICS_DATA.transferPool.buffers[0];
        VK_HANDLE_EXCEPT(vkResetCommandBuffer(command, 0));
        VK_HANDLE_EXCEPT(vkBeginCommandBuffer(command, &beginInfo));

        for (uint32_t i = 0; i < operationCount; i++)
        {
            auto& operation = pOperations[i];
            vkCmdCopyBuffer(command, operation.srcBuffer, operation.dstBuffer,
                            operation.regionCount, operation.pRegions);
        }

        VK_HANDLE_EXCEPT(vkEndCommandBuffer(command));
        VkSubmitInfo submitInfo
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &command
        };

        auto queue = GRAPHICS_DATA.defaultDevice->logicalDevice.transferQueue;
        vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(queue);

        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        for (uint32_t i = 0; i < operationCount; i++)
        {
            auto& operation = pOperations[i];
            if (operation.deleteSrcAfterCopy)
            {
                vkDestroyBuffer(device, operation.srcBuffer, nullptr);
                vkFreeMemory(device, operation.srcMemory, nullptr);
            }
        }
    }
}