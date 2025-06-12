#ifndef HVK_TEXTUREPACK_H
#define HVK_TEXTUREPACK_H

#include "hvk_shared.h"
#include "hvk_texture.h"

namespace hf
{
    struct VkTextureBinding
    {
        TextureSampler sampler{};
        std::vector<VkTexture*> textures{};
    };

    struct VkTexturePack
    {
        explicit VkTexturePack(const inter::rendering::TexturePackCreationInfo& info);
        ~VkTexturePack();

        RenderBindingType bindingType = RenderBindingType::Graphics;
        std::vector<VkTextureBinding> bindings{};
        VkDescriptorSet descriptors[FRAMES_IN_FLIGHT]{};
        TextureLayout layout = 0;
        uint32_t bindingId = 0;
    };

    struct VkTextureWriteOperation
    {
        std::vector<VkDescriptorImageInfo> imageInfos{};
        VkWriteDescriptorSet writes[FRAMES_IN_FLIGHT]{};
    };

    void UpdateTexturePack(const VkTexturePack* pack, uint32_t bindingOffset, uint32_t bindingCount);
    void SetTextureBinding(VkTexturePack* pack, uint32_t bindingIndex, TextureSampler sampler,
        VkTexture** pTextures, uint32_t offset, uint32_t size);
}

#endif //HVK_TEXTUREPACK_H
