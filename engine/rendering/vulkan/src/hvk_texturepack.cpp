#include "hvk_texturepack.h"
#include "hvk_graphics.h"

namespace hf
{
    VkTexturePack::VkTexturePack(const inter::rendering::TexturePackCreationInfo& info)
    {
        textures = std::vector<VkTexture>(info.textureCount);
        type = info.type;
        format = info.format;

        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        for (uint32_t i = 0; i < info.textureCount; i++)
        {
            auto& texInfo = info.pTextures[i];
            if (texInfo.data == nullptr) continue;

            auto& texture = textures[i];
            texture.size = texInfo.size;
            texture.channel = texInfo.channel;
            texture.mipLevels = texInfo.mipLevels;
            texture.bufferSize = texInfo.size.x * texInfo.size.y * texInfo.size.z * (uint64_t)texInfo.channel;
            texture.bufferOffset = 0;

            VkImageCreateInfo imageInfo
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                .imageType = (VkImageType)type,
                .extent = { texture.size.x, texture.size.y, texture.size.z },
                .mipLevels = texture.mipLevels,
                .arrayLayers = 1,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .tiling = VK_IMAGE_TILING_OPTIMAL,
                .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                .sharingMode = VK_SHARING_MODE_CONCURRENT,
                .format = (VkFormat)format,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .flags = 0,
            };

            VK_HANDLE_EXCEPT(vkCreateImage(device, &imageInfo, nullptr, &texture.image));

            AllocateImage(BufferMemoryType::Static, texture.image, &texture.imageMemory);

            VkBuffer stagingBuffer;
            VmaAllocation stagingBufferMemory;
            CreateStagingBuffer(texture.bufferSize, texInfo.data, &stagingBuffer, &stagingBufferMemory);

            VkBufferCopy copyRegion
            {
                .srcOffset = 0,
                .dstOffset = 0,
                .size = texture.bufferSize
            };

            VkCopyBufferToBufferOperation copyOperation
            {
                .srcBuffer = stagingBuffer,
                .srcMemory = stagingBufferMemory,
                .dstBuffer = *bufferResult,
                .dstMemory = *memoryResult,
                .regionCount = 1,
                .deleteSrcAfterCopy = true
            };
            copyOperation.pRegions[0] = copyRegion;
        }
    }

    VkTexturePack::~VkTexturePack()
    {
        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        for (auto& texture : textures)
        {
            vkDestroyImage(device, texture.image, nullptr);
            vmaDestroyImage(GRAPHICS_DATA.allocator, texture.image, texture.imageMemory);
        }

        textures.clear();
    }

    bool IsValidTexture(const VkTexture& texture)
    {
        return texture.image != VK_NULL_HANDLE;
    }
}
