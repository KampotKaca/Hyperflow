#ifndef HVK_TEXTUREPACK_H
#define HVK_TEXTUREPACK_H

#include "hvk_shared.h"

namespace hf
{
    struct VkTexture
    {
        VkImage image{};
        uvec3 size{};
        TextureChannel channel{};
        TextureFormat format{};
        uint64_t bufferOffset{};
        uint64_t bufferSize{};
        uint32_t mipLevels{};
        VmaAllocation imageMemory{};
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
