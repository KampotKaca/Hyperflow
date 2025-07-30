#include "hvk_buffer.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    VkBufferBase::VkBufferBase(const BufferMemoryType memoryType, const uint8_t* data,
    const VkMemoryPropertyFlags requiredFlags, const VmaAllocationCreateFlags allocationFlags, const VkBufferUsageFlags usage, const uint64_t bufferSize)
    : memoryType(memoryType), bufferSize(bufferSize)
    {
        switch (memoryType)
        {
            case BufferMemoryType::Static:
                {
                    if (!data) throw GENERIC_EXCEPT("[Hyperflow]", "Static buffer is without any data, this is not allowed");

                    VkStaticBufferInfo createInfo{};
                    createInfo.bufferSize = bufferSize;
                    createInfo.data = data;
                    createInfo.usage = usage;

                    CreateStaticBuffer(createInfo, &buffers[0], &memoryRegions[0]);
                    for (uint32_t i = 1; i < FRAMES_IN_FLIGHT; i++) buffers[i] = buffers[0];
                }
                break;
            case BufferMemoryType::WriteOnly:
                {
                    VkCreateBufferInfo bufferInfo{};
                    bufferInfo.size = bufferSize;
                    bufferInfo.usage = usage;
                    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                    bufferInfo.memoryType = memoryType;
                    bufferInfo.requiredFlags = requiredFlags;
                    bufferInfo.allocationFlags = allocationFlags;
                    bufferInfo.pQueueFamilies = nullptr;
                    bufferInfo.familyCount = 0;

                    CreateBuffer(bufferInfo, &buffers[0], &memoryRegions[0]);
                    VK_HANDLE_EXCEPT(vmaMapMemory(GRAPHICS_DATA.allocator, memoryRegions[0], &memoryMappings[0]));
                    for (uint32_t i = 1; i < FRAMES_IN_FLIGHT; i++)
                    {
                        buffers[i] = buffers[0];
                        memoryMappings[i] = memoryMappings[0];
                    }
                }
                break;
            case BufferMemoryType::PerFrameWriteOnly:
                {
                    for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++)
                    {
                        VkCreateBufferInfo bufferInfo{};
                        bufferInfo.size = bufferSize;
                        bufferInfo.usage = usage;
                        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                        bufferInfo.memoryType = memoryType;
                        bufferInfo.requiredFlags = requiredFlags;
                        bufferInfo.allocationFlags = allocationFlags;
                        bufferInfo.pQueueFamilies = nullptr;
                        bufferInfo.familyCount = 0;

                        CreateBuffer(bufferInfo, &buffers[i], &memoryRegions[i]);
                        VK_HANDLE_EXCEPT(vmaMapMemory(GRAPHICS_DATA.allocator, memoryRegions[i], &memoryMappings[i]));
                    }
                }
                break;
            default: throw GENERIC_EXCEPT("[Hyperflow]", "Unimplemented buffer memory type");
        }

        isLoaded = true;
    }

    VkBufferBase::~VkBufferBase()
    {
        if (isLoaded)
        {
            const uint32_t count = GetBufferCount(memoryType);
            for (uint32_t i = 0; i < count; i++)
            {
                vmaUnmapMemory(GRAPHICS_DATA.allocator, memoryRegions[i]);
                vmaDestroyBuffer(GRAPHICS_DATA.allocator, buffers[i], memoryRegions[i]);
            }

            isLoaded = false;
        }
    }

    bool IsValidBuffer(const Buffer buffer)
    {
        return buffer > 0 && buffer <= GRAPHICS_DATA.buffers.size();
    }

    URef<VkBufferBase>& GetBuffer(const Buffer buffer)
    {
        if (!IsValidBuffer(buffer)) throw GENERIC_EXCEPT("[Hyperflow]", "Invalid buffer");
        return GRAPHICS_DATA.buffers[buffer - 1];
    }

    void UploadBuffers(const VkRenderer* rn, const inter::rendering::BufferUploadInfo_i& info)
    {
        const auto currentFrame = rn->currentFrame;

        for (uint32_t i = 0; i < info.uploadPacketCount; i++)
        {
            auto& packet = info.pUploadPackets[i];
            const auto& buffer = GetBuffer(packet.buffer);
            memcpy((uint8_t*)buffer->memoryMappings[currentFrame] + packet.offsetInBytes,
                &info.pUniformDataBuffer[packet.bufferRange.start], packet.bufferRange.size);
        }
    }

    static void SetMemoryTypeFlags(BufferMemoryType memoryType, VmaAllocationCreateInfo* result);

    uint32_t GetMemoryType(const uint32_t filter, const VkMemoryPropertyFlags props)
    {
        const auto& memProps = GRAPHICS_DATA.device.memProps;
        for (uint32_t i = 0; i < memProps.memoryTypeCount; i++)
        {
            if ((filter & (1 << i)) && (memProps.memoryTypes[i].propertyFlags & props) == props) return i;
        }

        throw GENERIC_EXCEPT("[Hyperflow]", "Unable to allocate graphics memory");
    }

    void CreateBuffer(const VkCreateBufferInfo& info, VkBuffer* bufferResult, VmaAllocation* memResult)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = info.size;
        bufferInfo.usage = info.usage;
        bufferInfo.sharingMode = info.sharingMode;
        bufferInfo.queueFamilyIndexCount = info.familyCount;
        bufferInfo.pQueueFamilyIndices = info.pQueueFamilies;

        VmaAllocationCreateInfo vmaAllocInfo{};
        vmaAllocInfo.requiredFlags = info.requiredFlags;
        SetMemoryTypeFlags(info.memoryType, &vmaAllocInfo);
        vmaAllocInfo.flags |= info.allocationFlags;

        VmaAllocationInfo resultInfo{};
        VK_HANDLE_EXCEPT(vmaCreateBuffer(GRAPHICS_DATA.allocator, &bufferInfo, &vmaAllocInfo, bufferResult, memResult, &resultInfo));
    }

    void AllocateImage(const BufferMemoryType memoryType, const VkImage image, VmaAllocation* memResult)
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
        const QueueFamilyIndices& familyIndices = GRAPHICS_DATA.device.familyIndices;
        std::array queus =
        {
            familyIndices.transferFamily.value(),
            familyIndices.graphicsFamily.value()
        };

        VkCreateBufferInfo createInfo{};
        createInfo.size = bufferSize;
        createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.memoryType = BufferMemoryType::WriteOnly;
        createInfo.pQueueFamilies = queus.data();
        createInfo.familyCount = queus.size();

        CreateBuffer(createInfo, bufferResult, memoryResult);
        UploadBufferMemory(*memoryResult, data, 0, bufferSize);
    }

    void CreateStaticBuffer(const VkStaticBufferInfo& info, VkBuffer* bufferResult, VmaAllocation* memoryResult)
    {
        VkBuffer stagingBuffer{};
        VmaAllocation stagingBufferMemory{};
        CreateStagingBuffer(info.bufferSize, info.data, &stagingBuffer, &stagingBufferMemory);
        const QueueFamilyIndices& familyIndices = GRAPHICS_DATA.device.familyIndices;
        uint32_t queus[2]
        {
            familyIndices.transferFamily.value(),
            familyIndices.graphicsFamily.value()
        };

        VkCreateBufferInfo createInfo{};
        createInfo.size = info.bufferSize;
        createInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | info.usage;
        createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.memoryType = BufferMemoryType::Static;
        createInfo.pQueueFamilies = queus;
        createInfo.familyCount = 2;

        CreateBuffer(createInfo, bufferResult, memoryResult);

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = info.bufferSize;

        VkCopyBufferToBufferOperation copyOperation{};
        copyOperation.srcBuffer = stagingBuffer;
        copyOperation.srcMemory = stagingBufferMemory;
        copyOperation.dstBuffer = *bufferResult;
        copyOperation.dstMemory = *memoryResult;
        copyOperation.regionCount = 1;
        copyOperation.deleteSrcAfterCopy = true;

        copyOperation.pRegions[0] = copyRegion;
        StageCopyOperation(copyOperation);
    }

    void StageCopyOperation(const VkCopyBufferToBufferOperation& operation)
    {
        GRAPHICS_DATA.bufferToBufferCopyOperations.push_back(operation);
    }

    inline void CopyBufferToBuffer(VkCommandBuffer command)
    {
        for (const auto& operation : GRAPHICS_DATA.bufferToBufferCopyOperations)
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
                GRAPHICS_DATA.device.logicalDevice.transferQueue, CopyBufferToBuffer);

            for (const auto& operation : GRAPHICS_DATA.bufferToBufferCopyOperations)
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

    void UploadBufferMemory(const void* data, void* mapping, uint64_t fullOffset, uint64_t fullSize)
    {
        memcpy((uint8_t*)mapping + fullOffset, data, fullSize);
    }

    void BufferOperation(const VkCommandBuffer command, const VkQueue queue, void (*CopyCallback)(VkCommandBuffer command))
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        VK_HANDLE_EXCEPT(vkResetCommandBuffer(command, 0));
        VK_HANDLE_EXCEPT(vkBeginCommandBuffer(command, &beginInfo));

        CopyCallback(command);

        VK_HANDLE_EXCEPT(vkEndCommandBuffer(command));
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &command;

        VK_HANDLE_EXCEPT(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
        VK_HANDLE_EXCEPT(vkQueueWaitIdle(queue));
    }

    static void SetMemoryTypeFlags(const BufferMemoryType memoryType, VmaAllocationCreateInfo* result)
    {
        switch (memoryType)
        {
            case BufferMemoryType::Static:
                result->usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
                result->flags = 0;
                break;
            case BufferMemoryType::WriteOnly: case BufferMemoryType::PerFrameWriteOnly:
                result->usage = VMA_MEMORY_USAGE_AUTO;
                result->flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                                VMA_ALLOCATION_CREATE_MAPPED_BIT;
                break;
            case BufferMemoryType::ReadWrite: case BufferMemoryType::PerFrameReadWrite:
                result->usage = VMA_MEMORY_USAGE_AUTO;
                result->flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT |
                                VMA_ALLOCATION_CREATE_MAPPED_BIT;
                break;
            default: throw GENERIC_EXCEPT("[Hyperflow]", "Unknown memory type");
        }
    }
}
