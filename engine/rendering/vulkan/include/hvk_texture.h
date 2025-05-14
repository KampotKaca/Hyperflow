#ifndef HVK_TEXTURE_H
#define HVK_TEXTURE_H

#include "hvk_shared.h"

namespace hf
{
    struct VkTexture
    {
        explicit VkTexture(const inter::rendering::TextureCreationInfo& info);
        ~VkTexture();

        VkImage image{};
        VkImageView view{};
        VmaAllocation imageMemory{};

        TextureChannel channel{};
        TextureDetails details{};

        uvec3 size{};
        uint64_t bufferOffset{};
        uint64_t bufferSize{};
        uint32_t mipLevels = 1;
    };
}

#endif //HVK_TEXTURE_H
