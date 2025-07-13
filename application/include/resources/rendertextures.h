#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <hyperflow.h>

namespace app
{
    struct AppRenderTextures
    {
        hf::Ref<hf::RenderTexture> mainDrawRenderTexture{};
    };

    extern AppRenderTextures APP_RENDER_TEXTURES;

    void CreateRenderTextures();
}

#endif //RENDERPASS_H
