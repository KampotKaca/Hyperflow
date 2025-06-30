#include "hvk_buffer.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    VkBufferBase::VkBufferBase(const BufferDefinitionInfo& info, BufferMemoryType memoryType, const uint8_t* data,
        VkDescriptorType descriptorType, VkBufferUsageFlags usage)
    : memoryType(memoryType), bindingIndex(info.bindingId), descriptorType(descriptorType)
    {
        bindings = std::vector<BufferBindingInfo>(info.bindingCount);
        memcpy(bindings.data(), info.pBindings, sizeof(BufferBindingInfo) * info.bindingCount);
        bufferSize = 0;

        for (uint32_t i = 0; i < info.bindingCount; i++)
        {
            auto bindingInfo = info.pBindings[i];

            const VkDescriptorSetLayoutBinding layout
            {
                .binding = info.bindingId + i,
                .descriptorType = descriptorType,
                .descriptorCount = bindingInfo.arraySize,
                .stageFlags = (uint32_t)bindingInfo.usageFlags,
                .pImmutableSamplers = nullptr,
            };
            GRAPHICS_DATA.preAllocBuffers.descLayoutBindings[i] = layout;

            const auto offset = bindingInfo.elementSizeInBytes * bindingInfo.arraySize;
            if (i != info.bindingCount - 1 && offset % 256 != 0)
                LOG_ERROR("Invalid buffer alignment!!! "
                          "Some devices only support alignment 256, this is not recommended. "
                          "Alignment is only necessary when uniform has more than one binding");
            bufferSize += offset;
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = info.bindingCount,
            .pBindings = GRAPHICS_DATA.preAllocBuffers.descLayoutBindings,
        };

        VK_HANDLE_EXCEPT(vkCreateDescriptorSetLayout(GRAPHICS_DATA.device.logicalDevice.device,
            &layoutInfo, nullptr, &layout));

        switch (memoryType)
        {
            case BufferMemoryType::Static:
                {
                    if (!data) throw GENERIC_EXCEPT("[Hyperflow]", "Static buffer is without any data, this is not allowed");

                    const VkStaticBufferInfo createInfo
                    {
                        .bufferSize = bufferSize,
                        .data = data,
                        .usage = usage,
                    };

                    CreateStaticBuffer(createInfo, &buffers[0], &memoryRegions[0]);
                    for (uint32_t i = 1; i < FRAMES_IN_FLIGHT; i++) buffers[i] = buffers[0];
                }
                break;
            case BufferMemoryType::WriteOnly:
                {
                    const VkCreateBufferInfo bufferInfo
                    {
                        .size = bufferSize,
                        .usage = usage,
                        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                        .memoryType = memoryType,
                        .pQueueFamilies = nullptr,
                        .familyCount = 0,
                    };
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
                        VkCreateBufferInfo bufferInfo
                        {
                            .size = bufferSize,
                            .usage = usage,
                            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                            .memoryType = memoryType,
                            .pQueueFamilies = nullptr,
                            .familyCount = 0,
                        };
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
            uint32_t count = GetBufferCount(memoryType);
            for (uint32_t i = 0; i < count; i++)
            {
                vmaUnmapMemory(GRAPHICS_DATA.allocator, memoryRegions[i]);
                vmaDestroyBuffer(GRAPHICS_DATA.allocator, buffers[i], memoryRegions[i]);
            }

            vkDestroyDescriptorSetLayout(GRAPHICS_DATA.device.logicalDevice.device, layout, nullptr);
            isLoaded = false;
        }
    }

    bool IsValidBuffer(Buffer buffer)
    {
        return buffer > 0 && buffer <= GRAPHICS_DATA.buffers.size();
    }

    URef<VkBufferBase>& GetBuffer(Buffer buffer)
    {
        if (!IsValidBuffer(buffer)) throw GENERIC_EXCEPT("[Hyperflow]", "Invalid buffer");
        return GRAPHICS_DATA.buffers[buffer - 1];
    }

    void SetupBuffer(const URef<VkBufferBase>& buffer)
    {
        uint32_t totalWrites = 0, bufferOffset = 0, count = GetBufferCount(buffer->memoryType);

        for (uint32_t i = 0; i < buffer->bindings.size(); i++)
        {
            auto& binding = buffer->bindings[i];
            for (uint32_t j = 0; j < binding.arraySize; j++)
            {
                for (uint32_t frame = 0; frame < count; frame++)
                {
                    GRAPHICS_DATA.preAllocBuffers.bufferInfos[totalWrites] =
                    {
                        .buffer = buffer->buffers[frame],
                        .offset = bufferOffset,
                        .range = binding.elementSizeInBytes * binding.arraySize,
                    };

                    GRAPHICS_DATA.preAllocBuffers.descWrites[totalWrites] =
                    {
                        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                        .dstSet = buffer->descriptorSets[frame],
                        .dstBinding = buffer->bindingIndex + i,
                        .dstArrayElement = j,
                        .descriptorCount = 1,
                        .descriptorType = buffer->descriptorType,
                        .pImageInfo = nullptr,
                        .pBufferInfo = &GRAPHICS_DATA.preAllocBuffers.bufferInfos[totalWrites],
                        .pTexelBufferView = nullptr,
                    };

                    totalWrites++;
                }
            }

            bufferOffset += binding.elementSizeInBytes * binding.arraySize;
        }

        for (uint32_t i = count; i < FRAMES_IN_FLIGHT; i++)
            buffer->descriptorSets[i] = buffer->descriptorSets[0];

        vkUpdateDescriptorSets(GRAPHICS_DATA.device.logicalDevice.device,
                totalWrites, GRAPHICS_DATA.preAllocBuffers.descWrites,
                0, nullptr);
    }
    void UploadBuffers(const VkRenderer* rn, const inter::rendering::BufferUploadInfo& info)
    {
        auto currentFrame = rn->currentFrame;

        for (uint32_t i = 0; i < info.uploadPacketCount; i++)
        {
            auto& packet = info.pUploadPackets[i];
            auto& buffer = GetBuffer(packet.buffer);
            memcpy((uint8_t*)buffer->memoryMappings[currentFrame] + packet.offsetInBytes,
                &info.pUniformDataBuffer[packet.bufferRange.start], packet.bufferRange.size);
        }
    }
    void BindBuffers(const VkRenderer* rn, const inter::rendering::BufferBindInfo& info)
    {
        auto currentFrame = rn->currentFrame;

        for (uint32_t i = 0; i < info.bufferCount; i++)
        {
            const auto& buffer = GetBuffer(info.pBuffers[i]);
            GRAPHICS_DATA.preAllocBuffers.descriptors[i] = buffer->descriptorSets[currentFrame];
        }

        vkCmdBindDescriptorSets(rn->currentCommand, (VkPipelineBindPoint)info.bindingType, rn->currentLayout,
        info.setBindingIndex, info.bufferCount, GRAPHICS_DATA.preAllocBuffers.descriptors,
        0, nullptr);
    }

    static void SetMemoryTypeFlags(BufferMemoryType memoryType, VmaAllocationCreateInfo* result);

    uint32_t GetMemoryType(uint32_t filter, VkMemoryPropertyFlags props)
    {
        auto& memProps = GRAPHICS_DATA.device.memProps;
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
        QueueFamilyIndices& familyIndices = GRAPHICS_DATA.device.familyIndices;
        std::array queus = { familyIndices.transferFamily.value(), familyIndices.graphicsFamily.value() };

        const VkCreateBufferInfo createInfo
        {
            .size = bufferSize,
            .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            .sharingMode = VK_SHARING_MODE_CONCURRENT,
            .memoryType = BufferMemoryType::WriteOnly,
            .pQueueFamilies = queus.data(),
            .familyCount = queus.size()
        };

        CreateBuffer(createInfo, bufferResult, memoryResult);
        UploadBufferMemory(*memoryResult, data, 0, bufferSize);
    }

    void CreateStaticBuffer(const VkStaticBufferInfo& info, VkBuffer* bufferResult, VmaAllocation* memoryResult)
    {
        VkBuffer stagingBuffer;
        VmaAllocation stagingBufferMemory;
        CreateStagingBuffer(info.bufferSize, info.data, &stagingBuffer, &stagingBufferMemory);
        QueueFamilyIndices& familyIndices = GRAPHICS_DATA.device.familyIndices;
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
                GRAPHICS_DATA.device.logicalDevice.transferQueue, CopyBufferToBuffer);

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

    void UploadBufferMemory(const void* data, void* mapping, uint64_t fullOffset, uint64_t fullSize)
    {
        memcpy((uint8_t*)mapping + fullOffset, data, fullSize);
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

        VK_HANDLE_EXCEPT(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
        VK_HANDLE_EXCEPT(vkQueueWaitIdle(queue));
    }

    static void SetMemoryTypeFlags(BufferMemoryType memoryType, VmaAllocationCreateInfo* result)
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
