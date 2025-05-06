#include "hvk_texturepack.h"
#include "hvk_graphics.h"

namespace hf
{
    VkTexturePack::VkTexturePack(const inter::rendering::TexturePackCreationInfo& info)
    {
        textures = std::vector<VkTexture>(info.textureCount);
        type = info.type;

        QueueFamilyIndices& familyIndices = GRAPHICS_DATA.defaultDevice->familyIndices;
        uint32_t queus[2] = { familyIndices.transferFamily.value(), familyIndices.graphicsFamily.value() };
        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        for (uint32_t i = 0; i < info.textureCount; i++)
        {
            auto& texInfo = info.pTextures[i];
            if (texInfo.data == nullptr) continue;

            auto& texture = textures[i];
            texture.size = texInfo.size;
            texture.channel = texInfo.channel;
            texture.mipLevels = texInfo.mipLevels;
            texture.format = texInfo.format;
            texture.bufferSize = texInfo.size.x * texInfo.size.y * texInfo.size.z * 4;
            texture.bufferOffset = 0;

            VkImageCreateInfo imageInfo
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                .imageType = (VkImageType)type,
                .format = (VkFormat)texture.format,
                .extent = { texture.size.x, texture.size.y, texture.size.z },
                .mipLevels = texture.mipLevels,
                .arrayLayers = 1,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .tiling = VK_IMAGE_TILING_OPTIMAL,
                .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                .queueFamilyIndexCount = 2,
                .pQueueFamilyIndices = queus,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            };

            VK_HANDLE_EXCEPT(vkCreateImage(device, &imageInfo, nullptr, &texture.image));
            AllocateImage(BufferMemoryType::Static, texture.image, &texture.imageMemory);

            VkBuffer stagingBuffer;
            VmaAllocation stagingBufferMemory;
            CreateStagingBuffer(texture.bufferSize, texInfo.data, &stagingBuffer, &stagingBufferMemory);

            VkBufferImageCopy copyRegion
            {
                .bufferOffset = 0,
                .bufferRowLength = 0,
                .bufferImageHeight = 0,
                .imageSubresource =
                {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel = 0,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
                .imageOffset = { 0, 0, 0 },
                .imageExtent = { texture.size.x, texture.size.y, texture.size.z },
            };

            VkCopyBufferToImageOperation copyOperation
            {
                .srcBuffer = stagingBuffer,
                .srcMemory = stagingBufferMemory,
                .srcLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .dstImage = texture.image,
                .dstMemory = texture.imageMemory,
                .dstLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                .format = (VkFormat)texture.format,
                .regionCount = 1,
                .deleteSrcAfterCopy = true
            };

            copyOperation.pRegions[0] = copyRegion;
            StageCopyOperation(copyOperation);
        }

        SubmitBufferToImageCopyOperations();

        for (uint32_t i = 0; i < info.textureCount; i++)
        {
            auto& texture = textures[i];
            VkImageViewCreateInfo viewInfo
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = texture.image,
                .viewType = (VkImageViewType)info.type,
                .format = (VkFormat)texture.format,
                .subresourceRange =
                {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                }
            };

            VK_HANDLE_EXCEPT(vkCreateImageView(device, &viewInfo, nullptr, &textures[i].view));
        }
    }

    VkTexturePack::~VkTexturePack()
    {
        for (auto& texture : textures)
        {
            vkDestroyImageView(GRAPHICS_DATA.defaultDevice->logicalDevice.device, texture.view, nullptr);
            vmaDestroyImage(GRAPHICS_DATA.allocator, texture.image, texture.imageMemory);
        }

        textures.clear();
    }

    bool IsValidTexture(const VkTexture& texture)
    {
        return texture.image != VK_NULL_HANDLE;
    }
}
