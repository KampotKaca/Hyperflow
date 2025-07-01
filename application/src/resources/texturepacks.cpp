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
            hf::TexturePackTextureBindingInfo viking_room_binding
            {
                .sampler = APP_TEXTURE_SAMPLERS.defaultSampler,
                .pTextures = &APP_TEXTURES.viking_room,
                .arraySize = 1
            };

            const hf::TexturePackCreationInfo texPackInfo
            {
                .bindingType = hf::RenderBindingType::Graphics,
                .bindingId = 0,
                .pTextureBindings = &viking_room_binding,
                .textureBindingCount = 1,
                .layout = APP_TEXTURE_LAYOUTS.viking_room,
            };

            APP_TEXTURE_PACKS.viking_room_pack = hf::Create(texPackInfo);
        }

        hf::SubmitAllTexturePacks();

        //allocator
        {
            std::array packs { APP_TEXTURE_PACKS.viking_room_pack };
            hf::TexturePackAllocatorCreationInfo texPackAllocInfo
            {
                .pTexturePacks = packs.data(),
                .texturePackCount = packs.size()
            };

            APP_TEXTURE_PACKS.allocator = hf::Create(texPackAllocInfo);
        }
    }
}