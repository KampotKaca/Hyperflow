#include "hvk_graphics.h"
#include "hyperflow.h"

namespace hf
{
    static void SetMemoryTypeFlags(BufferMemoryType memoryType, VmaAllocationCreateInfo* result);

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
        SetMemoryTypeFlags(info.memoryType, &vmaAllocInfo);

        VmaAllocationInfo resultInfo{};
        VK_HANDLE_EXCEPT(vmaCreateBuffer(GRAPHICS_DATA.allocator, &bufferInfo, &vmaAllocInfo, bufferResult, memResult, &resultInfo));
    }

    void AllocateImage(BufferMemoryType memoryType, VkImage image, VmaAllocation* memResult)
    {
        VmaAllocationCreateInfo vmaAllocInfo{};
        switch (memoryType)
        {
            case BufferMemoryType::Static:
                vmaAllocInfo.usage = VMA_MEMORY_USAGE_UNKNOWN;
                vmaAllocInfo.flags = 0;
                vmaAllocInfo.preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

                break;
            case BufferMemoryType::WriteOnly:
                vmaAllocInfo.usage = VMA_MEMORY_USAGE_UNKNOWN;
                vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                                     VMA_ALLOCATION_CREATE_MAPPED_BIT;
                vmaAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

                break;
            case BufferMemoryType::ReadWrite:
                vmaAllocInfo.usage = VMA_MEMORY_USAGE_UNKNOWN;
                vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT |
                                     VMA_ALLOCATION_CREATE_MAPPED_BIT;
                vmaAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

                break;
            default: throw GENERIC_EXCEPT("[Hyperflow]", "Unknown memory type");
        }

        VmaAllocationInfo resultInfo{};
        VK_HANDLE_EXCEPT(vmaAllocateMemoryForImage(GRAPHICS_DATA.allocator, image, &vmaAllocInfo, memResult, &resultInfo));
        VK_HANDLE_EXCEPT(vmaBindImageMemory(GRAPHICS_DATA.allocator, *memResult, image));
    }

    void CreateStagingBuffer(uint64_t bufferSize, const void* data, VkBuffer* bufferResult, VmaAllocation* memoryResult)
    {
        QueueFamilyIndices& familyIndices = GRAPHICS_DATA.defaultDevice->familyIndices;
        uint32_t queus[2] = { familyIndices.transferFamily.value(), familyIndices.graphicsFamily.value() };

        VkCreateBufferInfo createInfo
        {
            .size = bufferSize,
            .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            .sharingMode = VK_SHARING_MODE_CONCURRENT,
            .memoryType = BufferMemoryType::WriteOnly,
            .pQueueFamilies = queus,
            .familyCount = 2
        };

        CreateBuffer(createInfo, bufferResult, memoryResult);
        UploadBufferMemory(*memoryResult, data, 0, bufferSize);
    }

    void CreateStaticBuffer(const VkStaticBufferInfo& info, VkBuffer* bufferResult, VmaAllocation* memoryResult)
    {
        VkBuffer stagingBuffer;
        VmaAllocation stagingBufferMemory;
        CreateStagingBuffer(info.bufferSize, info.data, &stagingBuffer, &stagingBufferMemory);
        QueueFamilyIndices& familyIndices = GRAPHICS_DATA.defaultDevice->familyIndices;
        uint32_t queus[2] = { familyIndices.transferFamily.value(), familyIndices.graphicsFamily.value() };

        VkCreateBufferInfo createInfo
        {
            .size = info.bufferSize,
            .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | info.usage,
            .sharingMode = VK_SHARING_MODE_CONCURRENT,
            .memoryType = BufferMemoryType::Static,
            .pQueueFamilies = queus,
            .familyCount = 2
        };

        CreateBuffer(createInfo, bufferResult, memoryResult);

        VkBufferCopy copyRegion
        {
            .srcOffset = 0,
            .dstOffset = 0,
            .size = info.bufferSize
        };

        VkCopyBufferToBufferOperation copyOperation
        {
            .srcBuffer = stagingBuffer,
            .srcMemory = stagingBufferMemory,
            .dstBuffer = *bufferResult,
            .dstMemory = *memoryResult,
            .regionCount = 1,
            .deleteSrcAfterCopy = true
        };
        copyOperation.pRegions[0] = copyRegion;
        StageCopyOperation(copyOperation);
    }

    void StageCopyOperation(const VkCopyBufferToBufferOperation& operation)
    {
        GRAPHICS_DATA.bufferToBufferCopyOperations.push_back(operation);
    }

    void StageCopyOperation(const VkCopyBufferToImageOperation& operation)
    {
        GRAPHICS_DATA.bufferToImageCopyOperations.push_back(operation);
    }

    inline void CopyBufferToBuffer(VkCommandBuffer command)
    {
        for (auto& operation : GRAPHICS_DATA.bufferToBufferCopyOperations)
        {
            vkCmdCopyBuffer(command, operation.srcBuffer, operation.dstBuffer,
                            operation.regionCount, operation.pRegions);
        }
    }

    void SubmitAllOperations()
    {
        SubmitBufferToBufferCopyOperations();
        SubmitBufferToImageCopyOperations();
    }

    void SubmitBufferToBufferCopyOperations()
    {
        if (!GRAPHICS_DATA.bufferToBufferCopyOperations.empty())
        {
            BufferOperation(GRAPHICS_DATA.transferPool.buffers[0],
                GRAPHICS_DATA.defaultDevice->logicalDevice.transferQueue, CopyBufferToBuffer);

            for (auto& operation : GRAPHICS_DATA.bufferToBufferCopyOperations)
            {
                if (operation.deleteSrcAfterCopy)
                    vmaDestroyBuffer(GRAPHICS_DATA.allocator, operation.srcBuffer, operation.srcMemory);
                if (operation.taskCompletionCallback) operation.taskCompletionCallback(operation.uData);
            }

            GRAPHICS_DATA.bufferToBufferCopyOperations.clear();
        }
    }

    void UploadBufferMemory(VmaAllocation memory, const void* data, uint64_t fullOffset, uint64_t fullSize)
    {
        void* mapping;
        VK_HANDLE_EXCEPT(vmaMapMemory(GRAPHICS_DATA.allocator, memory, &mapping));
        memcpy((uint8_t*)mapping + fullOffset, data, fullSize);
        vmaUnmapMemory(GRAPHICS_DATA.allocator, memory);
    }

    void BufferOperation(VkCommandBuffer command, VkQueue queue, void (*CopyCallback)(VkCommandBuffer command))
    {
        VkCommandBufferBeginInfo beginInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        };

        VK_HANDLE_EXCEPT(vkResetCommandBuffer(command, 0));
        VK_HANDLE_EXCEPT(vkBeginCommandBuffer(command, &beginInfo));

        CopyCallback(command);

        VK_HANDLE_EXCEPT(vkEndCommandBuffer(command));
        VkSubmitInfo submitInfo
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &command
        };

        vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(queue);
    }

    static void SetMemoryTypeFlags(BufferMemoryType memoryType, VmaAllocationCreateInfo* result)
    {
        switch (memoryType)
        {
            case BufferMemoryType::Static:
                result->usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
                result->flags = 0;
                break;
            case BufferMemoryType::WriteOnly:
                result->usage = VMA_MEMORY_USAGE_AUTO;
                result->flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                                     VMA_ALLOCATION_CREATE_MAPPED_BIT;
                break;
            case BufferMemoryType::ReadWrite:
                result->usage = VMA_MEMORY_USAGE_AUTO;
                result->flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT |
                                     VMA_ALLOCATION_CREATE_MAPPED_BIT;
                break;
            default: throw GENERIC_EXCEPT("[Hyperflow]", "Unknown memory type");
        }
    }
}
