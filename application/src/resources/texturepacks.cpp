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