#ifndef TEXTURELAYOUTS_H
#define TEXTURELAYOUTS_H

#include <hyperflow.h>

namespace app
{
    struct AppTextureLayouts
    {
        hf::TextureLayout default_lit{};
    };

    extern AppTextureLayouts APP_TEXTURE_LAYOUTS;

    void TextureLayoutDefineAll();
}

#endif //TEXTURELAYOUTS_H
