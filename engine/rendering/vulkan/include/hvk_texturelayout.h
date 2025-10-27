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
        SmallList<TextureLayoutBindingInfo, RN_NUM_MAX_TEXTURES_IN_LAYOUT> bindingInfos{};
    };

    bool IsValidLayout(TextureLayout layout);
    URef<VkTextureLayout>& GetTextureLayout(TextureLayout layout);
}

#endif //HVK_TEXTURELAYOUT_H
