#include "hvk_vertexbufferattribute.h"
#include "hvk_graphics.h"

namespace hf
{
    constexpr VkFormat BUFFER_FORMAT[(uint32_t)BufferDataType::Count * 4] =
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

    VkVertexBufferAttribute::VkVertexBufferAttribute(const VertexBufferAttributeDefinitionInfo& info, uint32_t fullStride)
    {
        bindingId = info.bindingId;
        attribCount = info.formatCount;
        vertexSize = fullStride;
        bindingDescription.binding = bindingId;
        bindingDescription.stride = fullStride;
        bindingDescription.inputRate = (VkVertexInputRate)info.inputRate;

        attribDescriptions = SmallList<VkVertexInputAttributeDescription, RN_NUM_MAX_VERTEX_ATTRIBUTES>(info.formatCount);

        uint32_t currentOffset = 0;
        uint32_t location = info.locationOffset;
        for (uint32_t i = 0; i < info.formatCount; i++)
        {
            auto& stride = info.pFormats[i];

            VkVertexInputAttributeDescription description{};
            description.location = location;
            description.binding = bindingId;
            description.format = BUFFER_FORMAT[(uint32_t)BufferDataType::Count * (stride.size - 1) + (uint32_t)stride.type];
            description.offset = currentOffset;

            attribDescriptions[i] = description;
            hassert(stride.lSize % 4 == 0, "[Hyperflow] Vertex buffer attribute size should be multiple of 4")

            location += ((stride.lSize - 1) / 16) + 1;
            currentOffset += stride.lSize;
        }
    }

    VkVertexBufferAttribute::~VkVertexBufferAttribute()
    {
        attribDescriptions.clear();
    }

    bool IsValidAttrib(VertexBufferAttribute attrib)
    {
        return attrib > 0 && attrib <= GRAPHICS_DATA.bufferAttribs.size();
    }

    URef<VkVertexBufferAttribute>& GetAttrib(VertexBufferAttribute attrib)
    {
        hassert(IsValidAttrib(attrib), "[Hyperflow] Invalid buffer attribute: %i", attrib);
        return GRAPHICS_DATA.bufferAttribs[attrib - 1];
    }
}
