#include "hvk_vertbuffer.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    static uint64_t GetVertBufferSize(VertexBufferAttribute attrib, uint32_t vertexCount)
    {
        const auto& bufferAttrib = GetAttrib(attrib);
        return (uint64_t)bufferAttrib->vertexSize * vertexCount;
    }

    VkVertBuffer::VkVertBuffer(const VertBufferCreationInfo& info) :
        VkBufferBase(info.memoryType, (uint8_t*)info.pVertices, 0, 0,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | (uint32_t)info.usageFlags, GetVertBufferSize(info.bufferAttrib, info.vertexCount)),
        vertCount(info.vertexCount), attrib(info.bufferAttrib)
    {

    }

    void UploadBuffer(const VkRenderer* rn, const VkVertBuffer* buffer, const void* data,
        uint32_t offset, uint32_t vertexCount)
    {
        uint32_t currentFrame = 0;
        if (rn) currentFrame = rn->currentFrame;

        if (buffer->memoryType == BufferMemoryType::Static)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot modify static buffer");

        const auto& attribute = GetAttrib(buffer->attrib);
        const auto fullSize = (uint64_t)attribute->vertexSize * vertexCount;
        const auto fullOffset = (uint64_t)offset * vertexCount;

        memcpy((uint8_t*)buffer->memoryMappings[currentFrame] + fullOffset, data, fullSize);
    }
}
