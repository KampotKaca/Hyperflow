#include "hvk_texture.h"
#include "hvk_graphics.h"
#include "hyperflow.h"

namespace hf
{
    void TransitionImageLayout(VkCommandBuffer command, const ImageTransitionInfo* pImageInfos, uint32_t imageInfoCount,
        VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t layerCount);
    static void TextureViewCallback(void* uData);
    static void GenerateMimMaps(VkCommandBuffer command);

    VkTexture::VkTexture(const ir::rdr::TextureCreationInfo_i& info)
        : type(info.type), viewType(info.viewType), flags(info.flags), channel(info.channel),
          details(info.details), size(info.size), bufferCount(info.textureCount)
    {
        bufferSize = size.x * size.y * size.z * 4;
        layout = (VkImageLayout)info.details.finalLayout;

        const auto device = GRAPHICS_DATA.device.logicalDevice.device;
        VkExtent3D extent{};
        extent.width  = size.x;
        extent.height = size.y;
        extent.depth  = size.z;

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.flags = (VkImageCreateFlags)info.flags;
        imageInfo.imageType = (VkImageType)info.type;
        imageInfo.format = (VkFormat)details.format;
        imageInfo.extent = extent;
        imageInfo.arrayLayers = info.textureCount;
        imageInfo.samples = (VkSampleCountFlagBits)info.samples;
        imageInfo.tiling = (VkImageTiling)details.tiling;

        if (info.pTextures)
        {
            mipLevels = std::min(info.mipLevels, (uint32_t)std::floor(std::log2(std::max(size.x, size.y))) + 1);

            if (!(GRAPHICS_DATA.device.formatProps[(uint32_t)details.format].optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
            {
                LOG_WARN("[Hyperflow]", "texture image format does not support linear blitting, disabling mimmaps!");
                return;
            }

            imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | (uint32_t)details.usageFlags;

            auto& transferData = GRAPHICS_DATA.device.transferData;
            imageInfo.queueFamilyIndexCount = (uint32_t)transferData.indices.size();
            imageInfo.pQueueFamilyIndices = transferData.indices.data();
            imageInfo.sharingMode = transferData.sharingMode;
        }
        else
        {
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.usage = (VkImageUsageFlags)details.usageFlags;
            mipLevels = 1;
        }

        imageInfo.mipLevels = mipLevels;

        VK_HANDLE_EXCEPT(vkCreateImage(device, &imageInfo, &GRAPHICS_DATA.platform.allocator, &image));
        AllocateImage(details.memoryType, image, &imageMemory);

        if (info.pTextures) QueueTextureUpload(this, info.pTextures, (VkImageAspectFlags)info.details.aspectFlags);
        else TextureViewCallback(this);
    }

    VkTexture::~VkTexture()
    {
        if (view) vkDestroyImageView(GRAPHICS_DATA.device.logicalDevice.device, view, &GRAPHICS_DATA.platform.allocator);
        vmaDestroyImage(GRAPHICS_DATA.allocator, image, imageMemory);
    }

    void QueueTextureUpload(VkTexture* tex, const void* pTextures, VkImageAspectFlags aspectFlags)
    {
        VkBuffer stagingBuffer;
        VmaAllocation stagingBufferMemory;
        CreateStagingBuffer(tex->bufferSize * tex->bufferCount, pTextures, &stagingBuffer, &stagingBufferMemory);

        VkBufferImageCopy copyRegion{};
        copyRegion.imageSubresource.aspectMask = aspectFlags;
        copyRegion.imageSubresource.layerCount = tex->bufferCount;
        copyRegion.imageExtent = VkExtent3D{ tex->size.x, tex->size.y, tex->size.z };

        VkCopyBufferToImageOperation copyOperation{};
        copyOperation.srcBuffer = stagingBuffer;
        copyOperation.srcMemory = stagingBufferMemory;
        copyOperation.dstMemory = tex->imageMemory;
        copyOperation.dstLayout = tex->layout;

        copyOperation.imageInfo.image = tex->image;
        copyOperation.imageInfo.format = (VkFormat)tex->details.format;
        copyOperation.imageInfo.aspectFlags = aspectFlags;
        copyOperation.imageInfo.mipLevels = tex->mipLevels;
        copyOperation.imageInfo.layerCount = tex->bufferCount;

        copyOperation.regionCount = 1;
        copyOperation.imageSize = tex->size;
        copyOperation.uData = tex;
        copyOperation.taskCompletionCallback = TextureViewCallback;
        copyOperation.deleteSrcAfterCopy = true;

        copyOperation.pRegions[0] = copyRegion;
        StageCopyOperation(copyOperation);
    }

    inline void TransitionImageLayout(VkCommandBuffer command, const ImageTransitionInfo* pImageInfos, uint32_t imageInfoCount,
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

        GRAPHICS_DATA.preAllocBuffers.imageBarriers.reserve(imageInfoCount);
        GRAPHICS_DATA.preAllocBuffers.imageBarriers.clear();

        for (uint32_t i = 0; i < imageInfoCount; i++)
        {
            auto& imageInfo = pImageInfos[i];

            VkImageMemoryBarrier2 barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
            barrier.srcAccessMask = srcAccessMask;
            barrier.dstAccessMask = dstAccessMask;
            barrier.srcStageMask = sourceStage;
            barrier.dstStageMask = destinationStage;
            barrier.oldLayout = oldLayout;
            barrier.newLayout = newLayout;
            barrier.image = imageInfo.image;

            barrier.subresourceRange.aspectMask = imageInfo.aspectFlags;
            barrier.subresourceRange.levelCount = imageInfo.mipLevels;
            barrier.subresourceRange.layerCount = imageInfo.layerCount;

            GRAPHICS_DATA.preAllocBuffers.imageBarriers.push_back(barrier);
        }

        VkDependencyInfo depInfo{};
        depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        depInfo.imageMemoryBarrierCount = GRAPHICS_DATA.preAllocBuffers.imageBarriers.size();
        depInfo.pImageMemoryBarriers = GRAPHICS_DATA.preAllocBuffers.imageBarriers.data();

        GRAPHICS_DATA.extensionFunctions.vkCmdPipelineBarrier2KHR(command, &depInfo);
    }

    inline void TransitionBufferToImageStart(VkCommandBuffer command)
    {
        for (auto& imageTransition : GRAPHICS_DATA.preAllocBuffers.imageTransitions) imageTransition.clear();

        for (uint32_t i = 0; i < GRAPHICS_DATA.bufferToImageCopyOperations.size(); i++)
        {
            const auto& operation = GRAPHICS_DATA.bufferToImageCopyOperations[i];
            auto& imageTransitionArray = GRAPHICS_DATA.preAllocBuffers.imageTransitions[operation.srcLayout];
            imageTransitionArray.push_back(operation.imageInfo);
        }

        for (uint32_t i = 0; i < 9; i++)
        {
            const auto& imageTransitionArray = GRAPHICS_DATA.preAllocBuffers.imageTransitions[i];
            if (imageTransitionArray.size() > 0)
            {
                TransitionImageLayout(command, imageTransitionArray.data(), imageTransitionArray.size(),
                (VkImageLayout)i, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            }
        }
    }

    void StageCopyOperation(const VkCopyBufferToImageOperation& operation)
    {
        GRAPHICS_DATA.bufferToImageCopyOperations.push_back(operation);
    }

    inline void CopyBufferToImage(VkCommandBuffer command)
    {
        for (const auto& operation : GRAPHICS_DATA.bufferToImageCopyOperations)
        {
            vkCmdCopyBufferToImage(command, operation.srcBuffer, operation.imageInfo.image,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                operation.regionCount, operation.pRegions);
        }
    }

    void SubmitBufferToImageCopyOperations()
    {
        if (!GRAPHICS_DATA.bufferToImageCopyOperations.empty())
        {
            const auto& device = GRAPHICS_DATA.device.logicalDevice;
            BufferOperation(GRAPHICS_DATA.transferPool.buffers[0], device.transferQueue, TransitionBufferToImageStart);
            BufferOperation(GRAPHICS_DATA.transferPool.buffers[0], device.transferQueue, CopyBufferToImage);
            BufferOperation(GRAPHICS_DATA.graphicsPool.buffers[0], device.graphicsQueue, GenerateMimMaps);

            for (const auto& operation : GRAPHICS_DATA.bufferToImageCopyOperations)
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
        VkImageViewCreateInfo viewInfo{};

        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = texture->image;
        viewInfo.viewType = (VkImageViewType)texture->viewType;
        viewInfo.format = (VkFormat)texture->details.format;

        viewInfo.subresourceRange.aspectMask = (VkImageAspectFlags)texture->details.aspectFlags;
        viewInfo.subresourceRange.levelCount = texture->mipLevels;
        viewInfo.subresourceRange.layerCount = texture->bufferCount;

        const auto device = GRAPHICS_DATA.device.logicalDevice.device;
        VK_HANDLE_EXCEPT(vkCreateImageView(device, &viewInfo, &GRAPHICS_DATA.platform.allocator, &texture->view));
    }

    void GenerateMimMaps(VkCommandBuffer command)
    {
        GRAPHICS_DATA.preAllocBuffers.imageBarriers.reserve(GRAPHICS_DATA.bufferToImageCopyOperations.size());
        GRAPHICS_DATA.preAllocBuffers.imageBarriers.clear();

        for (auto& operation : GRAPHICS_DATA.bufferToImageCopyOperations)
        {
            VkImageMemoryBarrier2 barrier{};

            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
            barrier.image = operation.imageInfo.image;

            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.layerCount = operation.imageInfo.layerCount;

            ivec2 size = operation.imageSize;
            for (uint32_t mip = 1; mip < operation.imageInfo.mipLevels; mip++)
            {
                barrier.subresourceRange.baseMipLevel = mip - 1;
                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;

                barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
                barrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;

                VkDependencyInfo depInfo{};
                depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
                depInfo.imageMemoryBarrierCount = 1;
                depInfo.pImageMemoryBarriers = &barrier;

                GRAPHICS_DATA.extensionFunctions.vkCmdPipelineBarrier2KHR(command, &depInfo);

                VkImageBlit blit{};
                blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.srcSubresource.mipLevel = mip - 1;
                blit.srcSubresource.layerCount = operation.imageInfo.layerCount;

                blit.srcOffsets[0] = VkOffset3D{ 0, 0, 0 };
                blit.srcOffsets[1] = VkOffset3D{ size.x, size.y, 1 };

                blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.dstSubresource.mipLevel = mip;
                blit.dstSubresource.layerCount = operation.imageInfo.layerCount;

                blit.dstOffsets[0] = VkOffset3D{ 0, 0, 0 };
                blit.dstOffsets[1] = VkOffset3D{ size.x > 1 ? size.x / 2 : 1, size.y > 1 ? size.y / 2 : 1, 1 };

                vkCmdBlitImage(command,
                operation.imageInfo.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                operation.imageInfo.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &blit, VK_FILTER_LINEAR);

                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.newLayout = operation.dstLayout;
                barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
                barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;

                barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
                barrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;

                GRAPHICS_DATA.extensionFunctions.vkCmdPipelineBarrier2KHR(command, &depInfo);

                if (size.x > 1) size.x /= 2;
                if (size.y > 1) size.y /= 2;
            }

            barrier.subresourceRange.baseMipLevel = operation.imageInfo.mipLevels - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = operation.dstLayout;
            barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;

            barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
            barrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;

            GRAPHICS_DATA.preAllocBuffers.imageBarriers.push_back(barrier);
        }

        VkDependencyInfo depInfo{};
        depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        depInfo.imageMemoryBarrierCount = GRAPHICS_DATA.preAllocBuffers.imageBarriers.size();
        depInfo.pImageMemoryBarriers = GRAPHICS_DATA.preAllocBuffers.imageBarriers.data();

        GRAPHICS_DATA.extensionFunctions.vkCmdPipelineBarrier2KHR(command, &depInfo);
    }
}
