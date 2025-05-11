#ifndef HVK_TEXTUREALLOCATOR_H
#define HVK_TEXTUREALLOCATOR_H

#include "hvk_shared.h"
#include "hvk_texture.h"

namespace hf
{
    struct VkTextureAllocator
    {
        explicit VkTextureAllocator(const inter::rendering::TextureAllocatorCreationInfo& info);
        ~VkTextureAllocator();

        std::vector<VkTexture*> textures{};
    };
}

#endif //HVK_TEXTUREALLOCATOR_H
