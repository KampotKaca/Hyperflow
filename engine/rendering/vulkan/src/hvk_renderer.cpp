#include "hvk_renderer.h"
#include "hvk_graphics.h"

namespace hf
{
    void DestroySurface(VKRenderer* rn)
    {
        vkDestroySurfaceKHR(GRAPHICS_DATA.instance, rn->swapchain.surface, nullptr);
        rn->swapchain.surface = VK_NULL_HANDLE;
    }

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
        auto command = info.renderer->currentCommand;
        vkCmdBindVertexBuffers(command, 0, info.bufferCount, info.pBuffers, info.pOffsets);
        if (info.indexBuffer)
        {
            vkCmdBindIndexBuffer(command, info.indexBuffer, 0, info.indexType);
            vkCmdDrawIndexed(command, info.indexCount, info.instanceCount, 0, 0, 0);
        }
        else vkCmdDraw(command, info.vertCount, info.instanceCount, 0, 0);
    }
}
