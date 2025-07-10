#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <hyperflow.h>

namespace app
{
    struct AppRenderTextures
    {
        hf::Ref<hf::RenderTexture> mainDrawRenderTexture{};
        // hf::Ref<hf::RenderTexture> presentationRenderTexture{};
    };

    extern AppRenderTextures APP_RENDER_TEXTURES;

    void CreateRenderTextures();
}

#endif //RENDERPASS_H
