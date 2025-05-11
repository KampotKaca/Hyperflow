#ifndef HVK_TEXTURELAYOUT_H
#define HVK_TEXTURELAYOUT_H

#include "hvk_shared.h"

namespace hf
{
    struct VkTextureLayout
    {
        explicit VkTextureLayout(const TextureLayoutDefinitionInfo& info);
        ~VkTextureLayout();

        VkDescriptorSetLayout layout{};
        std::vector<TextureLayoutBindingInfo> bindingInfos{};
    };

    bool IsValidLayout(TextureLayout layout);
    const VkTextureLayout& GetTextureLayout(TextureLayout layout);
}

#endif //HVK_TEXTURELAYOUT_H
