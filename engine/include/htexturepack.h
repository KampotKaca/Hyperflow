#ifndef HTEXTUREPACK_H
#define HTEXTUREPACK_H

#include "hshared.h"

namespace hf
{
    struct TexturePack
    {
        explicit TexturePack(const TexturePackCreationInfo& info);
        ~TexturePack();

        struct Binding
        {
            TextureSampler sampler{};
            std::vector<void*> textures{};
        };

        RenderBindingType bindingType = RenderBindingType::Graphics;
        std::vector<Binding> bindings{};
        std::vector<inter::rendering::TexturePackTextureBindingInfo> bindingsBuffer{};
        TextureLayout layout{};
        uint32_t bindingId = 0;
        void* handle{};
    };
}

#endif //HTEXTUREPACK_H
