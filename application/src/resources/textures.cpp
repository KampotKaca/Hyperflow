#include "resources/textures.h"
#include "resources/texturesamplers.h"
#include "resources/texturelayouts.h"

namespace app
{
    AppTextures APP_TEXTURES{};

    void TextureLoadAll()
    {
        APP_TEXTURES.viking_room = hf::texture::Create("viking_room.png");
        APP_TEXTURES.greek_head = hf::texture::Create("greek_head.jpg");
    }
}
