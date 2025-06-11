#ifndef TEXTUREPACKS_H
#define TEXTUREPACKS_H

#include <hyperflow.h>

namespace app
{
    struct AppTexturePacks
    {
        hf::Ref<hf::TexturePack> viking_room_pack;
        hf::Ref<hf::TexturePack> viking_room_pack2;
        hf::Ref<hf::TexturePackAllocator> allocator{};
    };

    extern AppTexturePacks APP_TEXTURE_PACKS;

    void TexturePackLoadAll();
}

#endif //TEXTUREPACKS_H
