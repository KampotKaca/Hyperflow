#include "resources/textures.h"
#include "resources/texturesamplers.h"
#include "resources/texturelayouts.h"

namespace app
{
    AppTextures APP_TEXTURES{};

    void TextureLoadAll()
    {
        APP_TEXTURES.viking_room = hf::Cast<hf::Texture>(hf::CreateAsset("viking_room.png", hf::AssetType::Texture));
        APP_TEXTURES.greek_head  = hf::Cast<hf::Texture>(hf::CreateAsset("greek_head.jpg",  hf::AssetType::Texture));
        hf::SubmitAllTextures();
    }
}
