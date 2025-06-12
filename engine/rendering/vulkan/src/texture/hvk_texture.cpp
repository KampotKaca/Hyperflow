#include "hvk_texture.h"
#include "hvk_graphics.h"
#include "hyperflow.h"

namespace hf
{
    void TransitionImageLayout(VkCommandBuffer command, const ImageTransitionInfo* pImageInfos, uint32_t imageInfoCount,
        VkImageLayout oldLayout, VkImageLayout newLayout);
    static void TextureViewCallback(void* uData);
    static void GenerateMimMaps(VkCommandBuffer command);

    VkTexture::VkTexture(const inter::rendering::TextureCreationInfo& info)
        : type(info.type), viewType(info.viewType), flags(info.flags), channel(info.channel),
          details(info.details), size(info.size)
    {
        bufferSize = size.x * size.y * size.z * 4;
        bufferCount = info.textureCount;
        bufferOffset = 0;

        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;

        VkImageCreateInfo imageInfo
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .flags = (VkImageCreateFlags)info.flags,
            .imageType = (VkImageType)info.type,
            .format = (VkFormat)details.format,
            .extent = { size.x, size.y, size.z },
            .arrayLayers = info.textureCount,
            .samples = (VkSampleCountFlagBits)info.samples,
            .tiling = (VkImageTiling)details.tiling,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        if (info.pTextures)
        {
            mipLevels = std::min(info.mipLevels, (uint32_t)std::floor(std::log2(std::max(size.x, size.y))) + 1);

            if (!(GRAPHICS_DATA.defaultDevice->formatProps[(uint32_t)details.format].optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
            {
                LOG_WARN("[Hyperflow]", "texture image format does not support linear blitting, disabling mimmaps!");
                return;
            }

            imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | (uint32_t)details.usageFlags;

            auto& transferData = GRAPHICS_DATA.defaultDevice->transferData;
            imageInfo.queueFamilyIndexCount = (uint32_t)transferData.indices.size();
            imageInfo.pQueueFamilyIndices = transferData.indices.data();
        }
        else
        {
            imageInfo.usage = (VkImageUsageFlags)details.usageFlags;
            mipLevels = 1;
        }

        imageInfo.mipLevels = mipLevels;

        VK_HANDLE_EXCEPT(vkCreateImage(device, &imageInfo, nullptr, &image));
        AllocateImage(details.memoryType, image, &imageMemory);

        if (info.pTextures)
        {
            VkBuffer stagingBuffer;
            VmaAllocation stagingBufferMemory;
            CreateStagingBuffer(bufferSize * bufferCount, info.pTextures, &stagingBuffer, &stagingBufferMemory);

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
                .mipLevels = mipLevels,
                .imageSize = size,
                .uData = this,
                .taskCompletionCallback = TextureViewCallback,
                .deleteSrcAfterCopy = true
            };

            copyOperation.pRegions[0] = copyRegion;
            StageCopyOperation(copyOperation);
        }
        else TextureViewCallback(this);
    }

    VkTexture::~VkTexture()
    {
        if (view) vkDestroyImageView(GRAPHICS_DATA.defaultDevice->logicalDevice.device, view, nullptr);
        vmaDestroyImage(GRAPHICS_DATA.allocator, image, imageMemory);
    }

    void TransitionImageLayout(VkCommandBuffer command, const ImageTransitionInfo* pImageInfos, uint32_t imageInfoCount,
        VkImageLayout oldLayout, VkImageLayout newLayout)
    {
        VkPipelineStageFlags sourceStage, destinationStage;
        VkAccessFlags srcAccessMask, dstAccessMask;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
            newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            srcAccessMask = 0;
            dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                 newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            srcAccessMask = 0;
            dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else throw GENERIC_EXCEPT("[Hyperflow]", "Unsupported layout transition!");

        for (uint32_t i = 0; i < imageInfoCount; i++)
        {
            auto& imageInfo = pImageInfos[i];
            auto& barrier = GRAPHICS_DATA.preAllocBuffers.imageBarriers[i];

            barrier =
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = srcAccessMask,
                .dstAccessMask = dstAccessMask,
                .oldLayout = oldLayout,
                .newLayout = newLayout,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = imageInfo.image,
                .subresourceRange
                {
                    .aspectMask = imageInfo.aspectFlags,
                    .baseMipLevel = 0,
                    .levelCount = imageInfo.mipLevels,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                },
            };
        }

        vkCmdPipelineBarrier(command, sourceStage, destinationStage,
            0, 0, nullptr,
            0, nullptr,
            imageInfoCount, GRAPHICS_DATA.preAllocBuffers.imageBarriers);
    }

    inline void TransitionBufferToImageStart(VkCommandBuffer command)
    {
        for (auto& imageTransition : GRAPHICS_DATA.preAllocBuffers.imageTransitions) imageTransition.count = 0;

        for (uint32_t i = 0; i < GRAPHICS_DATA.bufferToImageCopyOperations.size(); i++)
        {
            auto& operation = GRAPHICS_DATA.bufferToImageCopyOperations[i];
            auto& imageTransitionArray = GRAPHICS_DATA.preAllocBuffers.imageTransitions[operation.srcLayout];
            auto& imageTransition = imageTransitionArray.infos[imageTransitionArray.count];

            imageTransition =
            {
                .image = operation.dstImage,
                .format = operation.format,
                .aspectFlags = operation.aspectFlags,
                .mipLevels = operation.mipLevels
            };
            imageTransitionArray.count++;
        }

        for (uint32_t i = 0; i < 9; i++)
        {
            auto& imageTransitionArray = GRAPHICS_DATA.preAllocBuffers.imageTransitions[i];
            if (imageTransitionArray.count > 0)
            {
                TransitionImageLayout(command, imageTransitionArray.infos, imageTransitionArray.count,
                (VkImageLayout)i, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            }
        }
    }

    inline void CopyBufferToImage(VkCommandBuffer command)
    {
        for (auto& operation : GRAPHICS_DATA.bufferToImageCopyOperations)
        {
            vkCmdCopyBufferToImage(command, operation.srcBuffer, operation.dstImage,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                operation.regionCount, operation.pRegions);
        }
    }

    void SubmitBufferToImageCopyOperations()
    {
        if (!GRAPHICS_DATA.bufferToImageCopyOperations.empty())
        {
            auto& device = GRAPHICS_DATA.defaultDevice->logicalDevice;
            BufferOperation(GRAPHICS_DATA.transferPool.buffers[0], device.transferQueue, TransitionBufferToImageStart);
            BufferOperation(GRAPHICS_DATA.transferPool.buffers[0], device.transferQueue, CopyBufferToImage);
            BufferOperation(GRAPHICS_DATA.graphicsPool.buffers[0], device.graphicsQueue, GenerateMimMaps);

            for (auto& operation : GRAPHICS_DATA.bufferToImageCopyOperations)
            {
                if (operation.deleteSrcAfterCopy)
                    vmaDestroyBuffer(GRAPHICS_DATA.allocator, operation.srcBuffer, operation.srcMemory);
                if (operation.taskCompletionCallback) operation.taskCompletionCallback(operation.uData);
            }

            GRAPHICS_DATA.bufferToImageCopyOperations.clear();
        }
    }

    void TextureViewCallback(void* uData)
    {
        auto* texture = (VkTexture*)uData;
        VkImageViewCreateInfo viewInfo
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = texture->image,
            .viewType = (VkImageViewType)texture->viewType,
            .format = (VkFormat)texture->details.format,
            .subresourceRange =
            {
                .aspectMask = (VkImageAspectFlags)texture->details.aspectFlags,
                .baseMipLevel = 0,
                .levelCount = texture->mipLevels,
                .baseArrayLayer = 0,
                .layerCount = texture->bufferCount,
            }
        };

        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        VK_HANDLE_EXCEPT(vkCreateImageView(device, &viewInfo, nullptr, &texture->view));
    }

    void GenerateMimMaps(VkCommandBuffer command)
    {
        for (uint32_t i = 0; i < GRAPHICS_DATA.bufferToImageCopyOperations.size(); i++)
        {
            auto& operation = GRAPHICS_DATA.bufferToImageCopyOperations[i];
            VkImageMemoryBarrier barrier
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = VK_QUEUE_FAMILY_IGNORED,
                .dstAccessMask = VK_QUEUE_FAMILY_IGNORED,
                .image = operation.dstImage,
                .subresourceRange =
                {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                }
            };

            ivec2 size = operation.imageSize;
            for (uint32_t mip = 1; mip < operation.mipLevels; mip++)
            {
                barrier.subresourceRange.baseMipLevel = mip - 1;
                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

                vkCmdPipelineBarrier(command,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

                VkImageBlit blit
                {
                    .srcSubresource =
                    {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .mipLevel = mip - 1,
                        .baseArrayLayer = 0,
                        .layerCount = 1
                    },
                    .srcOffsets =
                    {
                        { 0, 0, 0 },
                        { size.x, size.y, 1 }
                    },
                    .dstSubresource =
                    {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .mipLevel = mip,
                        .baseArrayLayer = 0,
                        .layerCount = 1
                    },
                    .dstOffsets =
                    {
                        { 0, 0, 0 },
                        { size.x > 1 ? size.x / 2 : 1, size.y > 1 ? size.y / 2 : 1, 1 }
                    }
                };

                vkCmdBlitImage(command,
                operation.dstImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                operation.dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &blit, VK_FILTER_LINEAR);

                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.newLayout = operation.dstLayout;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                vkCmdPipelineBarrier(command,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

                if (size.x > 1) size.x /= 2;
                if (size.y > 1) size.y /= 2;
            }

            barrier.subresourceRange.baseMipLevel = operation.mipLevels - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = operation.dstLayout;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(command,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);
        }
    }
}
