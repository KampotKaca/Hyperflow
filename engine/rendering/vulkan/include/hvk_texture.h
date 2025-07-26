#ifndef HVK_TEXTURE_H
#define HVK_TEXTURE_H

#include "hvk_shared.h"

namespace hf
{
    struct VkTexture
    {
        explicit VkTexture(const inter::rendering::TextureCreationInfo_i& info);
        ~VkTexture();

        VkImage image{};
        VkImageView view{};
        VmaAllocation imageMemory{};

        inter::rendering::TextureType type = inter::rendering::TextureType::Tex2D;
        inter::rendering::TextureViewType viewType = inter::rendering::TextureViewType::Tex2D;
        inter::rendering::TextureFlags flags = inter::rendering::TextureFlags::None;
        TextureChannel channel{};
        TextureDetails details{};

        uvec3 size{};
        uint64_t bufferOffset{};
        uint64_t bufferSize{};
        uint32_t bufferCount = 1;
        uint32_t mipLevels = 1;
    };
}

#endif //HVK_TEXTURE_H
