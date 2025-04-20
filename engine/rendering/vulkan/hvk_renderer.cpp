#include "hvk_renderer.h"
#include "exceptions/hgraphicsexception.h"

#include "hrenderer.h"
#include "hvk_graphics.h"
#include "hinternal.h"

namespace hf::inter::rendering
{
    bool StartFrame(Renderer* rn)
    {
        auto renderer = (VKRenderer*)rn->handle;
        if (renderer->targetSize.x == 0 || renderer->targetSize.y == 0) return false;
        auto& frame = renderer->frames[renderer->currentFrame];
        if(!AcquireNextImage(renderer)) return false;

        frame.usedCommandCount = 0;
        BeginCommandBuffer(renderer, renderer->commandPool.buffers[renderer->currentFrame]);
        BeginRenderPass(renderer, GRAPHICS_DATA.renderPass);
        UploadViewportAndScissor(renderer);
        return true;
    }

    void EndFrame(Renderer* rn)
    {
        auto renderer = (VKRenderer*)rn->handle;
        EndRenderPass(renderer);
        EndCommandBuffer(renderer);
        SubmitCommands(renderer);
        PresentSwapchain(renderer);

        renderer->currentFrame = (renderer->currentFrame + 1) % renderer->frames.size();
    }

    void Draw(Renderer* rn)
    {
        auto renderer = (VKRenderer*)rn->handle;
        vkCmdDraw(renderer->currentCommand, 3, 1, 0, 0);
    }

    void RegisterFrameBufferChange(Renderer* rn, uvec2 newSize)
    {
        auto renderer = (VKRenderer*)rn->handle;
        renderer->targetSize = newSize;
        renderer->frameBufferResized = true;
    }
}
