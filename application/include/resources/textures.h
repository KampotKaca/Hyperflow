#ifndef TEXTURES_H
#define TEXTURES_H

#include <hyperflow.h>

namespace app
{
    struct AppTextures
    {
        hf::Ref<hf::Texture> viking_room;
        hf::Ref<hf::Texture> greek_head;
    };

    extern AppTextures APP_TEXTURES;

    void TextureLoadAll();
}

#endif //TEXTURES_H
