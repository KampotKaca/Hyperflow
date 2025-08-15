#include "resources/texturesamplers.h"

namespace app
{
    AppTextureSamplers APP_TEXTURE_SAMPLERS;

    void TextureSamplerDefineAll()
    {
        APP_TEXTURE_SAMPLERS.defaultSampler = hf::DefineTextureSamplerAsset("default");
    }
}