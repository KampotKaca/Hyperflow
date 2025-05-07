#ifndef HVK_TEXTURE_H
#define HVK_TEXTURE_H

#include "hvk_shared.h"

namespace hf
{
    struct VkTexture
    {
        VkTexture(const inter::rendering::TextureCreationInfo& info);
        ~VkTexture();

        VkImage image{};
        VkImageView view{};
        VmaAllocation imageMemory{};

        TextureType type{};
        TextureChannel channel{};
        TextureFormat format{};
        BufferMemoryType memoryType{};
        TextureSampler sampler{};

        uvec3 size{};
        uint64_t bufferOffset{};
        uint64_t bufferSize{};
        uint32_t mipLevels{};
    };

    void SetupTexture(const VkTexture* texture, uint32_t binding, const VkDescriptorSet* pDescriptors);
}

#endif //HVK_TEXTURE_H
