#ifndef HVK_TEXTUREPACK_H
#define HVK_TEXTUREPACK_H

#include "hvk_shared.h"
#include "hvk_texture.h"

namespace hf
{
    struct VkTextureBinding
    {
        TexturePackBindingType bindingType{};
        uint32_t bindingId = 0;
        TextureSampler sampler{};
        StaticVector<VkImageView, MAX_TEXTURES_IN_TEXTURE_PACK> views{};
    };

    struct VkTexturePack
    {
        explicit VkTexturePack(const inter::rendering::TexturePackCreationInfo& info);
        ~VkTexturePack();

        unordered_map<uint32_t, VkTextureBinding> bindings{};
        VkDescriptorSet descriptors[FRAMES_IN_FLIGHT]{};
        TextureLayout layout = 0;
    };

    struct VkTextureWriteOperation
    {
        std::vector<VkDescriptorImageInfo> imageInfos{};
        VkWriteDescriptorSet writes[FRAMES_IN_FLIGHT]{};
    };

    void UpdateTexturePack(VkTexturePack* pack, const uint32_t* bindingIndices, uint32_t bindingCount);
    void SetTextureBinding(VkTexturePack* pack, const inter::rendering::TexturePackBindingUploadInfo* bindings, uint32_t bindingCount);
}

#endif //HVK_TEXTUREPACK_H
