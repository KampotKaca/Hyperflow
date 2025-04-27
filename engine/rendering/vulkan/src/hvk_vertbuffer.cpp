#include "hvk_vertbuffer.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    VkVertBuffer::VkVertBuffer(const VertBufferCreationInfo& info)
    {
        attrib = info.bufferAttrib;
        vertCount = info.vertexCount;
        memoryType = info.memoryType;
        const auto& bufferAttrib = GetAttrib(attrib);
        uint64_t bufferSize = (uint64_t)bufferAttrib.vertexSize * info.vertexCount;

        VkCreateBufferInfo createInfo
        {
            .size = bufferSize,
            .pQueueFamilies = nullptr,
            .familyCount = 0
        };

        switch (memoryType)
        {
            case VertBufferMemoryType::Static:
            {
                if (!info.pVertices) throw GENERIC_EXCEPT("[Hyperflow]", "Static buffer is without any data, this is not allowed");

                QueueFamilyIndices& familyIndices = GRAPHICS_DATA.defaultDevice->familyIndices;
                uint32_t queus[2] = { familyIndices.transferFamily.value(), familyIndices.graphicsFamily.value() };

                createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
                createInfo.memoryType = VertBufferMemoryType::DynamicWrite;
                createInfo.pQueueFamilies = queus;
                createInfo.familyCount = 2;
                VkBuffer stagingBuffer;
                VkDeviceMemory stagingBufferMemory;
                CreateBuffer(createInfo, &stagingBuffer, &stagingBufferMemory);

                UploadBufferMemory(stagingBufferMemory, info.pVertices, 0, bufferSize);

                createInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
                createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
                createInfo.memoryType = VertBufferMemoryType::Static;
                CreateBuffer(createInfo, &buffer, &bufferMemory);

                VkBufferCopy copyRegion
                {
                    .srcOffset = 0,
                    .dstOffset = 0,
                    .size = bufferSize
                };

                VkCopyBufferOperation copyOperation
                {
                    .srcBuffer = stagingBuffer,
                    .srcMemory = stagingBufferMemory,
                    .dstBuffer = buffer,
                    .dstMemory = bufferMemory,
                    .regionCount = 1,
                    .deleteSrcAfterCopy = true
                };
                copyOperation.pRegions[0] = copyRegion;
                StageCopyOperation(copyOperation);
            }
            break;
            case VertBufferMemoryType::DynamicWrite:
            {
                createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
                createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                createInfo.memoryType = VertBufferMemoryType::DynamicWrite;
                CreateBuffer(createInfo, &buffer, &bufferMemory);
                if (info.pVertices) UploadBufferMemory(bufferMemory, info.pVertices, 0, bufferSize);
            }
            break;
            default: throw GENERIC_EXCEPT("[Hyperflow]", "Unknown memory type");
        }
    }

    VkVertBuffer::~VkVertBuffer()
    {
        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        vkDestroyBuffer(device, buffer, nullptr);
        vkFreeMemory(device, bufferMemory, nullptr);
    }

    void StageCopyOperation(const VkCopyBufferOperation& operation)
    {
        GRAPHICS_DATA.bufferCopyOperations.push_back(operation);
    }

    void SubmitStagedCopyOperations()
    {
        if (GRAPHICS_DATA.bufferCopyOperations.size() > 0)
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
