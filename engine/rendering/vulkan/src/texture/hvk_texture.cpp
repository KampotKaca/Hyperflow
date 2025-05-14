#include "hvk_texture.h"
#include "hvk_graphics.h"

namespace hf
{
    static void TextureViewCallback(void* uData);
    static void GenerateMimMaps(VkImage image, uvec2 texSize, uint32_t mipLevels);

    VkTexture::VkTexture(const inter::rendering::TextureCreationInfo& info)
        : channel(info.channel), details(info.details), size(info.size)
    {
        bufferSize = size.x * size.y * size.z * 4;
        bufferOffset = 0;

        QueueFamilyIndices& familyIndices = GRAPHICS_DATA.defaultDevice->familyIndices;
        uint32_t queus[2] = { familyIndices.transferFamily.value(), familyIndices.graphicsFamily.value() };
        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;

        VkImageCreateInfo imageInfo
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = (VkImageType)details.type,
            .format = (VkFormat)details.format,
            .extent = { size.x, size.y, size.z },
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = (VkImageTiling)details.tiling,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        if (info.data)
        {
            // mipLevels = glm::min(info.mipLevels, (uint32_t)glm::floor(glm::log2(glm::max(size.x, size.y))) + 1);

            imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | (uint32_t)details.usage;
            // if (mipLevels > 1) imageInfo.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

            imageInfo.queueFamilyIndexCount = 2;
            imageInfo.pQueueFamilyIndices = queus;
        }
        else
        {
            imageInfo.usage = (VkImageUsageFlags)details.usage;
            mipLevels = 1;
        }

        imageInfo.mipLevels = mipLevels;

        VK_HANDLE_EXCEPT(vkCreateImage(device, &imageInfo, nullptr, &image));
        AllocateImage(details.memoryType, image, &imageMemory);

        if (info.data)
        {
            VkBuffer stagingBuffer;
            VmaAllocation stagingBufferMemory;
            CreateStagingBuffer(bufferSize, info.data, &stagingBuffer, &stagingBufferMemory);

            VkBufferImageCopy copyRegion
            {
                .bufferOffset = 0,
                .bufferRowLength = 0,
                .bufferImageHeight = 0,
                .imageSubresource =
                {
                    .aspectMask = (VkImageAspectFlags)details.aspectFlags,
                    .mipLevel = 0,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
                .imageOffset = { 0, 0, 0 },
                .imageExtent = { size.x, size.y, size.z },
            };

            VkCopyBufferToImageOperation copyOperation
            {
                .srcBuffer = stagingBuffer,
                .srcMemory = stagingBufferMemory,
                .srcLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .dstImage = image,
                .dstMemory = imageMemory,
                .dstLayout = (VkImageLayout)info.details.finalLayout,
                .format = (VkFormat)details.format,
                .aspectFlags = (VkImageAspectFlags)info.details.aspectFlags,
                .regionCount = 1,
                .uData = this,
                .taskCompletionCallback = TextureViewCallback,
                .deleteSrcAfterCopy = true
            };

            copyOperation.pRegions[0] = copyRegion;
            StageCopyOperation(copyOperation);
        }
        else
        {
            TransitionEmptyImageLayout(image, (VkFormat)details.format,
                VK_IMAGE_LAYOUT_UNDEFINED, (VkImageLayout)info.details.finalLayout,
                (VkImageAspectFlags)info.details.aspectFlags);
            TextureViewCallback(this);
        }
    }

    VkTexture::~VkTexture()
    {
        if (view) vkDestroyImageView(GRAPHICS_DATA.defaultDevice->logicalDevice.device, view, nullptr);
        vmaDestroyImage(GRAPHICS_DATA.allocator, image, imageMemory);
    }

    void TextureViewCallback(void* uData)
    {
        auto* texture = (VkTexture*)uData;
        VkImageViewCreateInfo viewInfo
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = texture->image,
            .viewType = (VkImageViewType)texture->details.type,
            .format = (VkFormat)texture->details.format,
            .subresourceRange =
            {
                .aspectMask = (VkImageAspectFlags)texture->details.aspectFlags,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            }
        };

        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        VK_HANDLE_EXCEPT(vkCreateImageView(device, &viewInfo, nullptr, &texture->view));
    }

    void GenerateMimMaps(VkImage image, uvec2 texSize, uint32_t mipLevels)
    {
        VkImageMemoryBarrier barrier
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .image = image,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseArrayLayer = 0,
                .layerCount = 1,
                .levelCount = 1,
            }
        };
    }
}