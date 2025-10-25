#include "resources/texturepacks.h"
#include "resources/texturesamplers.h"
#include "resources/texturelayouts.h"

namespace app
{
    AppTexturePacks APP_TEXTURE_PACKS;

    void TexturePackLoadAll()
    {
        APP_TEXTURE_PACKS.white_pack = hf::Cast<hf::TexturePack>(hf::CreateAsset("white", hf::AssetType::TexturePack));
    }
}