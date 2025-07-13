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
            struct TextureInfo
            {
                Ref<T> texture{};
                uint32_t index{};
            };

            TextureSampler sampler{};
            StaticVector<TextureInfo, MAX_TEXTURES_IN_TEXTURE_ARRAY> textures{};
            uint32_t bindingIndex{};
        };

        StaticVector<Binding<Texture>, MAX_TEXTURES_IN_TEXTURE_PACK> textureBindings{};
        StaticVector<Binding<Cubemap>, MAX_TEXTURES_IN_TEXTURE_PACK> cubemapBindings{};
        StaticVector<Binding<RenderTexture>, MAX_TEXTURES_IN_TEXTURE_PACK> renderTextureBindings{};
        TextureLayout layout = 0;

        void* handle{};
    };
}

#endif //HTEXTUREPACK_H
