#ifndef HVK_TEXTUREPACK_H
#define HVK_TEXTUREPACK_H

#include "hvk_shared.h"
#include "hvk_texture.h"
#include "hvk_descriptorbuffer.h"

namespace hf
{
    struct VkTextureBindingViewData
    {
        VkImageView view{};
        VkImageLayout layout{};
        VkDescriptorLocation descriptorBindings[FRAMES_IN_FLIGHT]{};
    };

    struct VkTextureBinding
    {
        TexturePackBindingType bindingType{};
        uint32_t bindingId = 0;
        TextureSampler sampler{};
        SmallList<VkTextureBindingViewData, MAX_TEXTURES_IN_TEXTURE_PACK> bindingArray{};
    };

    struct VkTexturePack
    {
        explicit VkTexturePack(const inter::rendering::TexturePackCreationInfo_i& info);
        ~VkTexturePack();

        Dictionary<uint32_t, VkTextureBinding> bindings{};
        TextureLayout layout = 0;
    };

    void SetTextureBinding(VkTexturePack* pack, const inter::rendering::TexturePackBindingUploadInfo_i* bindings, uint32_t bindingCount);
}

#endif //HVK_TEXTUREPACK_H
