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
            case BufferMemoryType::Static:
            {
                if (!info.pVertices) throw GENERIC_EXCEPT("[Hyperflow]", "Static buffer is without any data, this is not allowed");

                QueueFamilyIndices& familyIndices = GRAPHICS_DATA.defaultDevice->familyIndices;
                uint32_t queus[2] = { familyIndices.transferFamily.value(), familyIndices.graphicsFamily.value() };

                createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
                createInfo.memoryType = BufferMemoryType::WriteOnly;
                createInfo.pQueueFamilies = queus;
                createInfo.familyCount = 2;
                VkBuffer stagingBuffer;
                VmaAllocation stagingBufferMemory;
                CreateBuffer(createInfo, &stagingBuffer, &stagingBufferMemory);
                UploadBufferMemory(stagingBufferMemory, info.pVertices, 0, bufferSize);

                createInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
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
            case BufferMemoryType::WriteOnly:
            {
                createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
                createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                createInfo.memoryType = BufferMemoryType::WriteOnly;
                CreateBuffer(createInfo, &buffer, &bufferMemory);
                if (info.pVertices) UploadBufferMemory(bufferMemory, info.pVertices, 0, bufferSize);
            }
            break;
            default: throw GENERIC_EXCEPT("[Hyperflow]", "Unknown memory type");
        }
    }

    VkVertBuffer::~VkVertBuffer()
    {
        vmaDestroyBuffer(GRAPHICS_DATA.allocator, buffer, bufferMemory);
    }
}
