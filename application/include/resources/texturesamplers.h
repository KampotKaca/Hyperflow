#ifndef TEXTURESAMPLERS_H
#define TEXTURESAMPLERS_H

#include <hyperflow.h>

namespace app
{
    struct AppTextureSamplers
    {
        hf::TextureSampler defaultSampler{};
    };

    extern AppTextureSamplers APP_TEXTURE_SAMPLERS;

    void TextureSamplerDefineAll();
}

#endif //TEXTURESAMPLERS_H
