#ifndef HVK_TEXTUREPACK_H
#define HVK_TEXTUREPACK_H

#include "hvk_shared.h"
#include "hvk_texture.h"

namespace hf
{
    struct VkPackedTexture
    {
        VkTexture* texture{};
        VkDescriptorSetLayout layout{};
        VkDescriptorSet descriptors[FRAMES_IN_FLIGHT]{};
    };

    struct VkTexturePack
    {
        VkTexturePack(const inter::rendering::TexturePackCreationInfo& info);
        ~VkTexturePack();

        std::vector<VkPackedTexture> textures{};
        VkDescriptorPool pool{};
        uint32_t bindingId = 0;
    };
}

#endif //HVK_TEXTUREPACK_H
