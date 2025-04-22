#include "include/hvk_renderer.h"
#include "include/hvk_graphics.h"

namespace hf
{
    bool StartFrame(VKRenderer* rn)
    {
        if (rn->targetSize.x == 0 || rn->targetSize.y == 0) return false;
        auto& frame = rn->frames[rn->currentFrame];
        if(!AcquireNextImage(rn)) return false;

        frame.usedCommandCount = 0;
        BeginCommandBuffer(rn, rn->commandPool.buffers[rn->currentFrame]);
        BeginRenderPass(rn, GRAPHICS_DATA.renderPass);
        UploadViewportAndScissor(rn);
        return true;
    }

    void EndFrame(VKRenderer* rn)
    {
        EndRenderPass(rn);
        EndCommandBuffer(rn);
        SubmitCommands(rn);
        PresentSwapchain(rn);

        rn->currentFrame = (rn->currentFrame + 1) % rn->frames.size();
    }

    void RegisterFrameBufferChange(VKRenderer* rn, uvec2 newSize)
    {
        rn->targetSize = newSize;
        rn->frameBufferResized = true;
    }

    void Draw(VKRenderer* rn)
    {
        vkCmdDraw(rn->currentCommand, 3, 1, 0, 0);
    }
}
