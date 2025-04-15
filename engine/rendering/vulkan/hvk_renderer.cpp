#include "exceptions/hgraphicsexception.h"

#include <hwindow.h>

#include "hrenderer.h"
#include "hvk_graphics.h"
#include "hinternal.h"

namespace hf::inter::rendering
{
    void StartFrame(const Ref<Renderer>& rn)
    {
        auto renderer = (VKRendererData*)rn->handle;
        BeginCommandBuffer(renderer, renderer->commandPool.buffers[0]);
        BeginRenderPass(GRAPHICS_DATA.renderPass, renderer);
    }

    void EndFrame(const Ref<Renderer>& rn)
    {
        auto renderer = (VKRendererData*)rn->handle;
        EndRenderPass(renderer);
        EndCommandBuffer(renderer);
    }
}
