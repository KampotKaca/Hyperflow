#include "resources/texturepacks.h"
#include "resources/texturesamplers.h"
#include "resources/texturelayouts.h"

namespace app
{
    AppTexturePacks APP_TEXTURE_PACKS;

    void TexturePackLoadAll()
    {
        {
            hf::TexturePackBindingInfo<hf::Texture>::TextureInfo white_info
            {
                .texture = hf::primitives::GetTexture(hf::PrimitiveTextureType::Default),
                .index = 0
            };

            hf::TexturePackBindingInfo<hf::Texture> white_binding
            {
                .sampler = APP_TEXTURE_SAMPLERS.defaultSampler,
                .textures = &white_info,
                .arraySize = 1,
                .bindingIndex = 0
            };

            const hf::TexturePackCreationInfo texPackInfo
            {
                .pTextureBindings = &white_binding,
                .textureBindingCount = 1,
                .layout = APP_TEXTURE_LAYOUTS.default_lit,
            };

            APP_TEXTURE_PACKS.white_pack = hf::Create(texPackInfo);
        }
    }
}