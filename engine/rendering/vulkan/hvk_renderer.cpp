#include "exceptions/hgraphicsexception.h"

#include <hwindow.h>

#include "hrenderer.h"
#include "hvk_graphics.h"
#include "hinternal.h"

namespace hf::inter::rendering
{
    bool StartFrame(Renderer* rn)
    {
        auto renderer = (VKRendererData*)rn->handle;
        if (renderer->targetSize.x == 0 || renderer->targetSize.y == 0) return false;
        auto& frame = renderer->frames[renderer->currentFrame];
        if(!AcquireNextImage(renderer)) return false;

        frame.usedCommandCount = 0;
        BeginCommandBuffer(renderer, renderer->commandPool.buffers[renderer->currentFrame]);
        BeginRenderPass(GRAPHICS_DATA.renderPass, renderer);
        UploadViewportAndScissor(renderer);
        return true;
    }

    void EndFrame(Renderer* rn)
    {
        auto renderer = (VKRendererData*)rn->handle;
        EndRenderPass(renderer);
        EndCommandBuffer(renderer);
        SubmitCommands(renderer);
        PresentSwapchain(renderer);

        renderer->currentFrame = (renderer->currentFrame + 1) % renderer->frames.size();
    }

    void Draw(Renderer* rn)
    {
        auto renderer = (VKRendererData*)rn->handle;
        vkCmdDraw(renderer->currentCommand, 3, 1, 0, 0);
    }

    void RegisterFrameBufferChange(Renderer* rn, uvec2 newSize)
    {
        auto renderer = (VKRendererData*)rn->handle;
        renderer->targetSize = newSize;
        renderer->frameBufferResized = true;
    }
}
