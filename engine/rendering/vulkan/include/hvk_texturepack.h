#ifndef HVK_TEXTUREPACK_H
#define HVK_TEXTUREPACK_H

#include "hvk_shared.h"

namespace hf
{
    struct VkTexture
    {
        VkImage image{};
        VkImageView view{};
        VmaAllocation imageMemory{};

        TextureChannel channel{};
        TextureFormat format{};

        uvec3 size{};
        uint64_t bufferOffset{};
        uint64_t bufferSize{};
        uint32_t mipLevels{};
    };

    struct VkTexturePack
    {
        VkTexturePack(const inter::rendering::TexturePackCreationInfo& info);
        ~VkTexturePack();

        std::vector<VkTexture> textures{};
        TextureType type{};
    };

    bool IsValidTexture(const VkTexture& texture);
}

#endif //HVK_TEXTUREPACK_H
