#include "hvk_indexbuffer.h"
#include "hvk_graphics.h"

namespace hf
{
    VkIndexBuffer::VkIndexBuffer(const IndexBufferCreationInfo& info)
    {
        indexCount = info.indexCount;
        memoryType = info.memoryType;
        indexFormat = info.indexFormat;

        switch (indexFormat)
        {
            case BufferDataType::U8:  indexType = VK_INDEX_TYPE_UINT8;  break;
            case BufferDataType::U16: indexType = VK_INDEX_TYPE_UINT16; break;
            case BufferDataType::U32: indexType = VK_INDEX_TYPE_UINT32; break;
            default: throw GENERIC_EXCEPT("[Hyperflow]", "Invalid index format!!!");
        }

        uint64_t bufferSize = (uint64_t)BUFFER_DATA_SIZE[(uint32_t)info.indexFormat] * info.indexCount;

        VkCreateBufferInfo createInfo
        {
            .size = bufferSize,
            .pQueueFamilies = nullptr,
            .familyCount = 0
        };

        switch (memoryType)
        {
            case BufferMemoryType::Static:
            {
                if (!info.pIndices) throw GENERIC_EXCEPT("[Hyperflow]", "Static buffer is without any data, this is not allowed");

                QueueFamilyIndices& familyIndices = GRAPHICS_DATA.defaultDevice->familyIndices;
                uint32_t queus[2] = { familyIndices.transferFamily.value(), familyIndices.graphicsFamily.value() };

                createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
                createInfo.memoryType = BufferMemoryType::DynamicWrite;
                createInfo.pQueueFamilies = queus;
                createInfo.familyCount = 2;
                VkBuffer stagingBuffer;
                VkDeviceMemory stagingBufferMemory;
                CreateBuffer(createInfo, &stagingBuffer, &stagingBufferMemory);

                UploadBufferMemory(stagingBufferMemory, info.pIndices, 0, bufferSize);

                createInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
                createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
                createInfo.memoryType = BufferMemoryType::Static;
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
            case BufferMemoryType::DynamicWrite:
            {
                createInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
                createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                createInfo.memoryType = BufferMemoryType::DynamicWrite;
                CreateBuffer(createInfo, &buffer, &bufferMemory);
                if (info.pIndices) UploadBufferMemory(bufferMemory, info.pIndices, 0, bufferSize);
            }
            break;
            default: throw GENERIC_EXCEPT("[Hyperflow]", "Unknown memory type");
        }
    }

    VkIndexBuffer::~VkIndexBuffer()
    {
        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        vkDestroyBuffer(device, buffer, nullptr);
        vkFreeMemory(device, bufferMemory, nullptr);
    }
}