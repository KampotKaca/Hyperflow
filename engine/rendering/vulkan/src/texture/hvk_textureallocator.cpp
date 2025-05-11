#include "hvk_textureallocator.h"
#include "hvk_graphics.h"
#include "hvk_texture.h"

namespace hf
{
    VkTextureAllocator::VkTextureAllocator(const inter::rendering::TextureAllocatorCreationInfo& info)
    {
        SubmitBufferToImageCopyOperations();

        textures = std::vector<VkTexture*>(info.textureCount);
        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        for (uint32_t i = 0; i < info.textureCount; i++)
        {
            auto texture = (VkTexture*)info.pTextures[i];
            textures[i] = texture;
            VkImageViewCreateInfo viewInfo
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = texture->image,
                .viewType = (VkImageViewType)texture->type,
                .format = (VkFormat)texture->format,
                .subresourceRange =
                {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                }
            };

            VK_HANDLE_EXCEPT(vkCreateImageView(device, &viewInfo, nullptr, &texture->view));
        }
    }

    VkTextureAllocator::~VkTextureAllocator()
    {
        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        for (auto& texture : textures)
            vkDestroyImageView(device, texture->view, nullptr);
    }
}
