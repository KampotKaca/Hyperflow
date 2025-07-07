#ifndef HRENDERTEXTURE_H
#define HRENDERTEXTURE_H

#include "hshared.h"

namespace hf
{
    struct RenderTexture
    {
        explicit RenderTexture(const RenderTextureCreationInfo& info);
        ~RenderTexture();

        RenderTextureCreationInfo createInfo{};
        void* handle{};
    };
}

#endif //HRENDERTEXTURE_H
