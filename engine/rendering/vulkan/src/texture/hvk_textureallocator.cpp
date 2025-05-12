#include "hvk_textureallocator.h"
#include "hvk_graphics.h"
#include "hvk_texture.h"

namespace hf
{
    VkTextureAllocator::VkTextureAllocator(const inter::rendering::TextureAllocatorCreationInfo& info)
    {
        SubmitBufferToImageCopyOperations();

        textures = std::vector<VkTexture*>(info.textureCount);
        for (uint32_t i = 0; i < info.textureCount; i++)
        {
            auto texture = (VkTexture*)info.pTextures[i];
            textures[i] = texture;
            CreateTextureView(texture);
        }
    }

    VkTextureAllocator::~VkTextureAllocator()
    {
        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        for (auto& texture : textures)
            vkDestroyImageView(device, texture->view, nullptr);
    }
}
