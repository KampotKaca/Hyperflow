#include "resources/textures.h"
#include "resources/texturesamplers.h"
#include "resources/texturelayouts.h"

namespace app
{
    AppTextures APP_TEXTURES{};

    void TextureLoadAll()
    {
        APP_TEXTURES.viking_room = hf::CreateTextureAsset("viking_room.png");
        APP_TEXTURES.greek_head = hf::CreateTextureAsset("greek_head.jpg");
        APP_TEXTURES.white = hf::CreateTextureAsset("white.png");
        hf::SubmitAllTextures();
    }
}
