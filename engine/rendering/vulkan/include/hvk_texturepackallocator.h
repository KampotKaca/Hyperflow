#ifndef HVK_TEXTUREPACKALLOCATOR_H
#define HVK_TEXTUREPACKALLOCATOR_H

#include "hvk_shared.h"

namespace hf
{
    struct VkTexturePackAllocator
    {
        explicit VkTexturePackAllocator(const inter::rendering::TexturePackAllocatorCreationInfo_i& info);
        ~VkTexturePackAllocator();

        VkDescriptorPool pool{};
    };
}

#endif //HVK_TEXTUREPACKALLOCATOR_H
