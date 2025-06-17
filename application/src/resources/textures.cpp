#include "resources/textures.h"
#include "resources/texturesamplers.h"
#include "resources/texturelayouts.h"

namespace app
{
    AppTextures APP_TEXTURES{};

    void TextureLoadAll()
    {
        APP_TEXTURES.viking_room = hf::Texture::Create("viking_room.png");
        APP_TEXTURES.greek_head = hf::Texture::Create("greek_head.jpg");
    }
}
