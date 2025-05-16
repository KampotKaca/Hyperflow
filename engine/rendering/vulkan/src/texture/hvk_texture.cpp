#include "hvk_texture.h"
#include "hvk_graphics.h"

namespace hf
{
    void TransitionEmptyImageLayout(const ImageTransitionInfo& transitionInfo, VkImageLayout srcLayout, VkImageLayout dstLayout);
    void TransitionImageLayout(VkCommandBuffer command, const ImageTransitionInfo* pImageInfos, uint32_t imageInfoCount,
        VkImageLayout oldLayout, VkImageLayout newLayout);
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
            mipLevels = std::min(info.mipLevels, (uint32_t)std::floor(std::log2(std::max(size.x, size.y))) + 1);

            if (mipLevels > 1)
            {
                if (!(GRAPHICS_DATA.defaultDevice->formatProps[(uint32_t)details.format].optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
                {
                    LOG_WARN("[Hyperflow]", "texture image format does not support linear blitting, disabling mimmaps!");
                    mipLevels = 1;
                }
            }

            imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | (uint32_t)details.usage;
            if (mipLevels > 1) imageInfo.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

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
                .mipLevels = mipLevels,
                .uData = this,
                .taskCompletionCallback = TextureViewCallback,
                .deleteSrcAfterCopy = true
            };

            copyOperation.pRegions[0] = copyRegion;
            StageCopyOperation(copyOperation);
        }
        else
        {
            ImageTransitionInfo transitionInfo
            {
                .image = image,
                .format = (VkFormat)details.format,
                .aspectFlags = (VkImageAspectFlags)info.details.aspectFlags,
                .mipLevels = mipLevels
            };

            TransitionEmptyImageLayout(transitionInfo, VK_IMAGE_LAYOUT_UNDEFINED, (VkImageLayout)info.details.finalLayout);
            TextureViewCallback(this);
        }
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
                    .baseMipLevel = imageInfo.mipLevels - 1,
                    .levelCount = 1,
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
        for (uint32_t i = 0; i < 9; i++) GRAPHICS_DATA.preAllocBuffers.imageTransitions[i].count = 0;

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

    inline void TransitionBufferToImageEnd(VkCommandBuffer command)
    {
        for (uint32_t i = 0; i < 9; i++) GRAPHICS_DATA.preAllocBuffers.imageTransitions[i].count = 0;

        for (uint32_t i = 0; i < GRAPHICS_DATA.bufferToImageCopyOperations.size(); i++)
        {
            auto& operation = GRAPHICS_DATA.bufferToImageCopyOperations[i];
            auto& imageTransitionArray = GRAPHICS_DATA.preAllocBuffers.imageTransitions[operation.dstLayout];
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
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, (VkImageLayout)i);
            }
        }
    }

    void SubmitBufferToImageCopyOperations()
    {
        if (!GRAPHICS_DATA.bufferToImageCopyOperations.empty())
        {
            auto& device = GRAPHICS_DATA.defaultDevice->logicalDevice;
            BufferOperation(GRAPHICS_DATA.transferPool.buffers[0], device.transferQueue, TransitionBufferToImageStart);
            BufferOperation(GRAPHICS_DATA.transferPool.buffers[0], device.transferQueue, CopyBufferToImage);
            BufferOperation(GRAPHICS_DATA.graphicsPool.buffers[0], device.graphicsQueue, TransitionBufferToImageEnd);

            for (auto& operation : GRAPHICS_DATA.bufferToImageCopyOperations)
            {
                if (operation.deleteSrcAfterCopy)
                    vmaDestroyBuffer(GRAPHICS_DATA.allocator, operation.srcBuffer, operation.srcMemory);
                if (operation.taskCompletionCallback) operation.taskCompletionCallback(operation.uData);
            }

            GRAPHICS_DATA.bufferToImageCopyOperations.clear();
        }
    }

    void TransitionEmptyImageLayout(const ImageTransitionInfo& transitionInfo, VkImageLayout srcLayout, VkImageLayout dstLayout)
    {
        auto command = GRAPHICS_DATA.graphicsPool.buffers[0];
        VkCommandBufferBeginInfo beginInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        };

        VK_HANDLE_EXCEPT(vkResetCommandBuffer(command, 0));
        VK_HANDLE_EXCEPT(vkBeginCommandBuffer(command, &beginInfo));

        TransitionImageLayout(command, &transitionInfo, 1, srcLayout, dstLayout);

        VK_HANDLE_EXCEPT(vkEndCommandBuffer(command));
        VkSubmitInfo submitInfo
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &command
        };

        auto& queue = GRAPHICS_DATA.defaultDevice->logicalDevice.graphicsQueue;
        VK_HANDLE_EXCEPT(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
        VK_HANDLE_EXCEPT(vkQueueWaitIdle(queue));
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
                .levelCount = texture->mipLevels,
                .baseArrayLayer = 0,
                .layerCount = 1,
            }
        };

        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        VK_HANDLE_EXCEPT(vkCreateImageView(device, &viewInfo, nullptr, &texture->view));
    }

    void GenerateMimMaps(VkImage image, uvec2 texSize, uint32_t mipLevels)
    {
        auto command = GRAPHICS_DATA.graphicsPool.buffers[0];
        VkCommandBufferBeginInfo beginInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        };

        VK_HANDLE_EXCEPT(vkResetCommandBuffer(command, 0));
        VK_HANDLE_EXCEPT(vkBeginCommandBuffer(command, &beginInfo));

        VkImageMemoryBarrier barrier
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask = VK_QUEUE_FAMILY_IGNORED,
            .dstAccessMask = VK_QUEUE_FAMILY_IGNORED,
            // .oldLayout = ,
            // .newLayout = ,
            // .srcQueueFamilyIndex = ,
            // .dstQueueFamilyIndex = ,
            .image = image,
            .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .levelCount = mipLevels,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        ivec2 tSize = texSize;
        for (uint32_t i = 1; i < mipLevels; i++)
        {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(command,
                    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                    0, nullptr,
                    0, nullptr,
                    1, &barrier);
        }
    }
}