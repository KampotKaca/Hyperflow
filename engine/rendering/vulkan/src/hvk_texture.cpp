#include "hvk_texture.h"
#include "hvk_graphics.h"

namespace hf
{
    VkTexture::VkTexture(const inter::rendering::TextureCreationInfo& info)
    {
        type = info.type;
        size = info.size;
        channel = info.channel;
        mipLevels = info.mipLevels;
        format = info.format;
        memoryType = info.memoryType;
        sampler = info.sampler;
        bufferSize = size.x * size.y * size.z * 4;
        bufferOffset = 0;

        QueueFamilyIndices& familyIndices = GRAPHICS_DATA.defaultDevice->familyIndices;
        uint32_t queus[2] = { familyIndices.transferFamily.value(), familyIndices.graphicsFamily.value() };
        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;

        VkImageCreateInfo imageInfo
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = (VkImageType)type,
            .format = (VkFormat)info.format,
            .extent = { size.x, size.y, size.z },
            .mipLevels = mipLevels,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 2,
            .pQueueFamilyIndices = queus,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        VK_HANDLE_EXCEPT(vkCreateImage(device, &imageInfo, nullptr, &image));
        AllocateImage(memoryType, image, &imageMemory);

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
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
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
            .dstLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .format = (VkFormat)format,
            .regionCount = 1,
            .deleteSrcAfterCopy = true
        };

        copyOperation.pRegions[0] = copyRegion;
        StageCopyOperation(copyOperation);
    }

    VkTexture::~VkTexture()
    {
        vmaDestroyImage(GRAPHICS_DATA.allocator, image, imageMemory);
    }

    void SetupTexture(const VkTexture* texture, uint32_t binding, const VkDescriptorSet* pDescriptors)
    {
        if (!IsValidSampler(texture->sampler)) throw GENERIC_EXCEPT("[Hyperflow]", "Invalid texture sampler");
        auto& texSampler = GRAPHICS_DATA.textureSamplers[texture->sampler - 1];

        auto& device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            VkDescriptorImageInfo imageInfo
            {
                .sampler = texSampler.sampler,
                .imageView = texture->view,
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            };

            VkWriteDescriptorSet descriptorWrite
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = pDescriptors[i],
                .dstBinding = binding,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &imageInfo,
            };

            vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
        }
    }
}