#include "resources/texturepacks.h"
#include "resources/texturesamplers.h"
#include "resources/texturelayouts.h"
#include "resources/textures.h"

namespace app
{
    AppTexturePacks APP_TEXTURE_PACKS;

    void TexturePackLoadAll()
    {
        //viking_room_pack
        {
            hf::TexturePackBindingInfo<hf::Texture>::TextureInfo viking_texture_info
            {
                .texture = APP_TEXTURES.viking_room,
                .index = 0
            };

            hf::TexturePackBindingInfo<hf::Texture> viking_room_binding
            {
                .sampler = APP_TEXTURE_SAMPLERS.defaultSampler,
                .textures = &viking_texture_info,
                .arraySize = 1,
                .bindingIndex = 0
            };

            const hf::TexturePackCreationInfo texPackInfo
            {
                .pTextureBindings = &viking_room_binding,
                .textureBindingCount = 1,
                .layout = APP_TEXTURE_LAYOUTS.default_lit,
            };

            APP_TEXTURE_PACKS.viking_room_pack = hf::Create(texPackInfo);
        }

        {
            hf::TexturePackBindingInfo<hf::Texture>::TextureInfo white_info
            {
                .texture = APP_TEXTURES.white,
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