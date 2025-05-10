#ifndef HTEXTUREPACK_H
#define HTEXTUREPACK_H

#include "hshared.h"

namespace hf
{
    struct TexturePackBinding
    {
        TexturePackBinding() = default;
        ~TexturePackBinding() = default;
        uint32_t bindingId = 0;
        TextureSampler sampler{};
        std::vector<Ref<Texture>> textures{};
    };

    struct TexturePack
    {
        TexturePack(const TexturePackCreationInfo& info);
        ~TexturePack();

        RenderBindingType bindingType = RenderBindingType::Graphics;
        std::vector<TexturePackBinding> bindings{};
        TextureLayout layout{};
        uint32_t setBindingIndex = 0;
        void* handle{};
    };
}

#endif //HTEXTUREPACK_H
