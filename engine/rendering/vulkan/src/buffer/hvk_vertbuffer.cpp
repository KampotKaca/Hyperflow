#include "hvk_vertbuffer.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    VkVertBuffer::VkVertBuffer(const VertBufferCreationInfo& info)
        : vertCount(info.vertexCount), attrib(info.bufferAttrib), memoryType(info.memoryType)
    {
        const auto& bufferAttrib = GetAttrib(attrib);
        uint64_t bufferSize = (uint64_t)bufferAttrib.vertexSize * info.vertexCount;

        switch (memoryType)
        {
            case BufferMemoryType::Static:
                {
                    if (!info.pVertices) throw GENERIC_EXCEPT("[Hyperflow]", "Static buffer is without any data, this is not allowed");

                    VkStaticBufferInfo createInfo
                    {
                        .bufferSize = bufferSize,
                        .data = info.pVertices,
                        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | (uint32_t)info.usageFlags,
                    };

                    CreateStaticBuffer(createInfo, &buffer, &bufferMemory);
                }
            break;
            case BufferMemoryType::WriteOnly:
                {
                    VkCreateBufferInfo createInfo
                    {
                        .size = bufferSize,
                        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | (uint32_t)info.usageFlags,
                        .memoryType = BufferMemoryType::WriteOnly,
                        .pQueueFamilies = nullptr,
                        .familyCount = 0,
                    };

                    CreateBuffer(createInfo, &buffer, &bufferMemory);
                    VK_HANDLE_EXCEPT(vmaMapMemory(GRAPHICS_DATA.allocator, bufferMemory, &mapping));
                    if (info.pVertices) UploadBufferMemory((const void*)info.pVertices, mapping, 0, bufferSize);
                }
            break;
            default: throw GENERIC_EXCEPT("[Hyperflow]", "Unknown memory type");
        }
    }

    VkVertBuffer::~VkVertBuffer()
    {
        if (mapping)
        {
            vmaUnmapMemory(GRAPHICS_DATA.allocator, bufferMemory);
            mapping = nullptr;
        }
        vmaDestroyBuffer(GRAPHICS_DATA.allocator, buffer, bufferMemory);
    }
}
