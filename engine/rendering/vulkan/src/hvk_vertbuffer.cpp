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

        switch (memoryType)
        {
            case BufferMemoryType::Static:
            {
                if (!info.pVertices) throw GENERIC_EXCEPT("[Hyperflow]", "Static buffer is without any data, this is not allowed");

                VkStaticBufferInfo createInfo
                {
                    .bufferSize = bufferSize,
                    .data = info.pVertices,
                    .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                };

                CreateStaticBuffer(createInfo, &buffer, &bufferMemory);
            }
            break;
            case BufferMemoryType::WriteOnly:
            {
                VkCreateBufferInfo createInfo
                {
                    .size = bufferSize,
                    .pQueueFamilies = nullptr,
                    .familyCount = 0,
                    .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                    .memoryType = BufferMemoryType::WriteOnly,
                };

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
