#include "hvk_vertexbuffer.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    VkVertexBuffer::VkVertexBuffer(const ir::rdr::VertexBufferCreationInfo_i& info) :
        VkBufferBase(info.memoryType, (uint8_t*)info.pVertices, 0, 0,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | (uint32_t)info.usageFlags, info.sizeInBytes)
    {

    }

    void UploadBuffer(const VkRenderer* rn, const VkVertexBuffer* buffer, const void* data,
        uint32_t offsetInBytes, uint32_t sizeInBytes)
    {
        uint32_t currentFrame = 0;
        if (rn) currentFrame = rn->currentFrame;

        if (buffer->memoryType == BufferMemoryType::Static)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot modify static buffer");

        memcpy((uint8_t*)buffer->memoryMappings[currentFrame] + offsetInBytes, data, sizeInBytes);
    }
}
