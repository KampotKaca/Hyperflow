#include "hvk_vertbuffer.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    VkVertBuffer::VkVertBuffer(const VertBufferCreationInfo& info)
    {
        attrib = info.bufferAttrib;
        vertCount = info.vertexCount;
        enableReadWrite = info.enableReadWrite;
        const auto& bufferAttrib = GetAttrib(attrib);
        uint64_t fullSize = (uint64_t)bufferAttrib.vertexSize * info.vertexCount;

        VkBufferCreateInfo bufferInfo
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = fullSize,
            .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE
        };

        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        VK_HANDLE_EXCEPT(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer));

        VkMemoryRequirements memReqs{};
        vkGetBufferMemoryRequirements(device, buffer, &memReqs);
        VkMemoryAllocateInfo allocInfo
        {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memReqs.size,
            .memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits,
                                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
        };

        VK_HANDLE_EXCEPT(vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory));
        VK_HANDLE_EXCEPT(vkBindBufferMemory(device, buffer, bufferMemory, 0));

        void* data;
        VK_HANDLE_EXCEPT(vkMapMemory(device, bufferMemory, 0, bufferInfo.size, 0, &data));
        memcpy(data, info.pVertices, fullSize);
        vkUnmapMemory(device, bufferMemory);
    }

    VkVertBuffer::~VkVertBuffer()
    {
        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        vkDestroyBuffer(device, buffer, nullptr);
        vkFreeMemory(device, bufferMemory, nullptr);
    }
}
