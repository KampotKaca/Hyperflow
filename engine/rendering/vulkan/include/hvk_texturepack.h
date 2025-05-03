#ifndef HVK_TEXTUREPACK_H
#define HVK_TEXTUREPACK_H

#include "hvk_shared.h"

namespace hf
{
    struct VkTexturePack
    {
        VkTexturePack(const inter::rendering::TexturePackCreationInfo& info);
        ~VkTexturePack();
    };
}

#endif //HVK_TEXTUREPACK_H
