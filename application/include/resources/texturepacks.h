#ifndef TEXTUREPACKS_H
#define TEXTUREPACKS_H

#include <hyperflow.h>

namespace app
{
    struct AppTexturePacks
    {
        hf::Ref<hf::TexturePack> white_pack;
    };

    extern AppTexturePacks APP_TEXTURE_PACKS;

    void TexturePackLoadAll();
}

#endif //TEXTUREPACKS_H
