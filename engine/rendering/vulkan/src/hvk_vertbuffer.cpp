#include "hvk_vertbuffer.h"
#include "hvk_graphics.h"

namespace hf
{
    VkVertBuffer::VkVertBuffer(const VertBufferCreationInfo& info)
    {
        attrib = info.bufferAttrib;
        vertCount = info.vertexCount;
        memoryType = info.memoryType;
        const auto& bufferAttrib = GetAttrib(attrib);
        uint64_t fullSize = (uint64_t)bufferAttrib.vertexSize * info.vertexCount;

        VkCreateBufferInfo createInfo
        {
            .size = fullSize,
            .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .memoryType = memoryType
        };

        CreateBuffer(createInfo, &buffer, &bufferMemory);

        switch (memoryType)
        {
            case VertBufferMemoryType::Static:
                break;
            case VertBufferMemoryType::DynamicWrite:
                if (info.pVertices) UploadVertBuffer(this, info.pVertices, 0, info.vertexCount);
                break;
            case VertBufferMemoryType::DynamicReadWrite:
                if (info.pVertices) UploadVertBuffer(this, info.pVertices, 0, info.vertexCount);
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

    void UploadVertBuffer(const VkVertBuffer* buffer, const void* data, uint32_t offset, uint32_t vertCount)
    {
        if (buffer->memoryType == VertBufferMemoryType::Static)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot modify static buffer");

        auto& attribute = GetAttrib(buffer->attrib);
        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        auto fullSize = attribute.vertexSize * vertCount;
        void* mapping;
        VK_HANDLE_EXCEPT(vkMapMemory(device, buffer->bufferMemory, offset * vertCount, fullSize, 0, &mapping));
        memcpy(mapping, data, fullSize);
        vkUnmapMemory(device, buffer->bufferMemory);
    }
}
