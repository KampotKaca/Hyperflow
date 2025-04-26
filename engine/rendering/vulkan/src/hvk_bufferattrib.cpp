#include "hvk_bufferattrib.h"
#include "hvk_graphics.h"

namespace hf
{
    constexpr uint32_t BUFFER_SIZE[(uint32_t)VertBufferDataType::Count] = { 1, 1, 2, 2, 4, 4, 8, 8, 2, 4, 8 };
    constexpr VkFormat BUFFER_FORMAT[(uint32_t)VertBufferDataType::Count * 4] =
    {
        VK_FORMAT_R8_UINT, VK_FORMAT_R8_SINT, VK_FORMAT_R16_UINT, VK_FORMAT_R16_SINT,
        VK_FORMAT_R32_UINT, VK_FORMAT_R32_SINT, VK_FORMAT_R64_UINT, VK_FORMAT_R64_SINT,
        VK_FORMAT_R16_SFLOAT, VK_FORMAT_R32_SFLOAT, VK_FORMAT_R64_SFLOAT,

        VK_FORMAT_R8G8_UINT, VK_FORMAT_R8G8_SINT, VK_FORMAT_R16G16_UINT, VK_FORMAT_R16G16_SINT,
        VK_FORMAT_R32G32_UINT, VK_FORMAT_R32G32_SINT, VK_FORMAT_R64G64_UINT, VK_FORMAT_R64G64_SINT,
        VK_FORMAT_R16G16_SFLOAT, VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R64G64_SFLOAT,

        VK_FORMAT_R8G8B8_UINT, VK_FORMAT_R8G8B8_SINT, VK_FORMAT_R16G16B16_UINT, VK_FORMAT_R16G16B16_SINT,
        VK_FORMAT_R32G32B32_UINT, VK_FORMAT_R32G32B32_SINT, VK_FORMAT_R64G64B64_UINT, VK_FORMAT_R64G64B64_SINT,
        VK_FORMAT_R16G16B16_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R64G64B64_SFLOAT,

        VK_FORMAT_R8G8B8A8_UINT, VK_FORMAT_R8G8B8A8_SINT, VK_FORMAT_R16G16B16A16_UINT, VK_FORMAT_R16G16B16A16_SINT,
        VK_FORMAT_R32G32B32A32_UINT, VK_FORMAT_R32G32B32A32_SINT, VK_FORMAT_R64G64B64A64_UINT, VK_FORMAT_R64G64B64A64_SINT,
        VK_FORMAT_R16G16B16A16_SFLOAT, VK_FORMAT_R32G32B32A32_SFLOAT, VK_FORMAT_R64G64B64A64_SFLOAT,
    };

    VkBufferAttrib::VkBufferAttrib(const BufferAttribCreateInfo& info, uint32_t fullStride)
    {
        attribCount = info.formatCount;
        bindingDescription =
        {
            .binding = 0,
            .stride = fullStride,
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        };

        attribDescriptions = std::vector<VkVertexInputAttributeDescription>(info.formatCount);

        uint32_t currentOffset = 0;
        uint32_t location = 0;
        for (uint32_t i = 0; i < info.formatCount; i++)
        {
            auto& stride = info.pFormats[i];
            VkVertexInputAttributeDescription description =
            {
                .location = location,
                .binding = 0,
                .format = BUFFER_FORMAT[(uint32_t)VertBufferDataType::Count * (stride.size - 1) + (uint32_t)stride.type],
                .offset = currentOffset
            };
            attribDescriptions[i] = description;
            auto currentSize = stride.size * BUFFER_SIZE[(uint32_t)stride.type];
            if (currentSize % 4 != 0)
                throw GENERIC_EXCEPT("[Hyperflow]", "Vertex buffer attribute size should be multiple of 4");

            if (currentSize > 16) location++;
            currentOffset += currentSize;
            location++;
        }
    }

    VkBufferAttrib::~VkBufferAttrib()
    {
        attribDescriptions.clear();
    }
}
