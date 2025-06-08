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
            hf::TexturePackBindingInfo viking_room_binding
            {
                .sampler = APP_TEXTURE_SAMPLERS.defaultSampler,
                .pTextures = &APP_TEXTURES.viking_room,
                .arraySize = 1
            };

            hf::TexturePackCreationInfo texPackInfo
            {
                .bindingType = hf::RenderBindingType::Graphics,
                .bindingId = 0,
                .setBindingIndex = 2,
                .pBindings = &viking_room_binding,
                .bindingCount = 1,
                .layout = APP_TEXTURE_LAYOUTS.viking_room,
            };

            APP_TEXTURE_PACKS.viking_room_pack = hf::texturepack::Create(texPackInfo);
        }

        hf::texture::SubmitAll();

        //allocator
        {
            hf::TexturePackAllocatorCreationInfo texPackAllocInfo
            {
                .pTexturePacks = &APP_TEXTURE_PACKS.viking_room_pack,
                .texturePackCount = 1
            };

            APP_TEXTURE_PACKS.allocator = hf::texturepackallocator::Create(texPackAllocInfo);
        }
    }
}