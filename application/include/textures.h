#ifndef TEXTURES_H
#define TEXTURES_H

#include <hyperflow.h>

namespace app
{
    struct AppTextures
    {
        hf::TextureSampler sampler{};
        hf::TextureLayout viking_room_layout{};

        hf::Ref<hf::Texture> viking_room;
        hf::Ref<hf::Texture> greek_head;
        hf::Ref<hf::TexturePack> viking_room_pack;
        hf::Ref<hf::TexturePackAllocator> allocator{};
    };

    extern AppTextures APP_TEXTURES;

    void TextureDefineAll();
    void TextureLoadAll();
}

#endif //TEXTURES_H
