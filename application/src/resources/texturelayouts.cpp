#include "resources/texturelayouts.h"

namespace app
{
    AppTextureLayouts APP_TEXTURE_LAYOUTS;

    void TextureLayoutDefineAll()
    {
        APP_TEXTURE_LAYOUTS.default_lit = hf::DefineTextureLayoutAsset("default_lit");
    }
}