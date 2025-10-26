#ifndef HTEXTUREPACK_H
#define HTEXTUREPACK_H

#include "hshared.h"
#include "hinternal.h"

namespace hf
{
    struct TexturePack
    {
        explicit TexturePack(const TexturePackCreationInfo& info);
        ~TexturePack();

        template<typename T>
        struct Binding
        {
            struct Info
            {
                Ref<T> texture{};
                uint32_t index{};
            };

            TextureSampler sampler{};
            StaticList<Info, MAX_TEXTURES_IN_TEXTURE_ARRAY> textures{};
            uint32_t bindingIndex{};
        };

        StaticList<Binding<Texture>, MAX_TEXTURES_IN_TEXTURE_PACK> textureBindings{};
        StaticList<Binding<Cubemap>, MAX_TEXTURES_IN_TEXTURE_PACK> cubemapBindings{};
        StaticList<Binding<RenderTexture>, MAX_TEXTURES_IN_TEXTURE_PACK> renderTextureBindings{};
        TextureLayout layout = 0;

        void* handle{};
    };
}

#endif //HTEXTUREPACK_H
