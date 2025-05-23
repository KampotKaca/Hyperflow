#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <hyperflow.h>

namespace app
{
    struct AppRenderPasses
    {
        hf::RenderPass mainPresentPass{};
    };

    extern AppRenderPasses APP_RENDER_PASSES;

    hf::RenderPass DefineMainPresentationPass(const hf::Ref<hf::Renderer>& rn);
}

#endif //RENDERPASS_H
