#ifndef HTEXTUREPACK_H
#define HTEXTUREPACK_H

#include "hshared.h"

namespace hf
{
    struct TexturePack
    {
        TexturePack(const TexturePackCreationInfo& info);
        ~TexturePack();

        uint32_t bindingId = 0;
        UniformBufferStage usageStage = UniformBufferStage::Vertex | UniformBufferStage::Fragment;
        std::vector<Ref<Texture>> textures{};
        void* handle{};
    };
}

#endif //HTEXTUREPACK_H
