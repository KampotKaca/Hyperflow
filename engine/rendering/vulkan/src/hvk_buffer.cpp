#include "hvk_graphics.h"

namespace hf
{
    uint32_t GetMemoryType(uint32_t filter, VkMemoryPropertyFlags props)
    {
        auto& memProps = GRAPHICS_DATA.defaultDevice->memProps;
        for (uint32_t i = 0; i < memProps.memoryTypeCount; i++)
        {
            if ((filter & (1 << i)) && (memProps.memoryTypes[i].propertyFlags & props) == props) return i;
        }

        throw GENERIC_EXCEPT("[Hyperflow]", "Unable to allocate graphics memory");
    }

    void CreateBuffer(const VkCreateBufferInfo& info, VkBuffer* bufferResult, VmaAllocation* memResult)
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

        VmaAllocationCreateInfo vmaAllocInfo{};
        switch (info.memoryType)
        {
            case BufferMemoryType::Static:
                vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
                vmaAllocInfo.flags = 0;
                break;
            case BufferMemoryType::WriteOnly:
                vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
                vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                                     VMA_ALLOCATION_CREATE_MAPPED_BIT;
                break;
            case BufferMemoryType::ReadWrite:
                vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
                vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT |
                                     VMA_ALLOCATION_CREATE_MAPPED_BIT;
                break;
            default: throw GENERIC_EXCEPT("[Hyperflow]", "Unknown memory type");
        }

        VmaAllocationInfo resultInfo{};
        VK_HANDLE_EXCEPT(vmaCreateBuffer(GRAPHICS_DATA.allocator, &bufferInfo, &vmaAllocInfo, bufferResult, memResult, &resultInfo));
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

    void UploadBufferMemory(VmaAllocation memory, const void* data, uint64_t fullOffset, uint64_t fullSize)
    {
        void* mapping;
        VK_HANDLE_EXCEPT(vmaMapMemory(GRAPHICS_DATA.allocator, memory, &mapping));
        memcpy((u_char*)mapping + fullOffset, data, fullSize);
        vmaUnmapMemory(GRAPHICS_DATA.allocator, memory);
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

        for (uint32_t i = 0; i < operationCount; i++)
        {
            auto& operation = pOperations[i];
            if (operation.deleteSrcAfterCopy)
                vmaDestroyBuffer(GRAPHICS_DATA.allocator, operation.srcBuffer, operation.srcMemory);
        }
    }
}