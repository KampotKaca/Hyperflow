#include "hvk_renderer.h"
#include "hvk_graphics.h"

namespace hf
{
    bool GetReadyForRendering(VKRenderer* rn)
    {
        SubmitStagedCopyOperations();
        if (rn->targetSize.x == 0 || rn->targetSize.y == 0) return false;
        return AcquireNextImage(rn);
    }

    void StartFrame(VKRenderer* rn)
    {
        auto& frame = rn->frames[rn->currentFrame];
        frame.usedCommandCount = 0;
        BeginCommandBuffer(rn, rn->commandPool.buffers[rn->currentFrame]);
        BeginRenderPass(rn, GRAPHICS_DATA.renderPass);
        UploadViewportAndScissor(rn);
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

    void Draw(const VkDrawInfo& info)
    {
        vkCmdBindVertexBuffers(info.renderer->currentCommand, 0, info.bufferCount, info.pBuffers, info.pOffsets);
        vkCmdDraw(info.renderer->currentCommand, info.vertCount, info.instanceCount, 0, 0);
    }
}
