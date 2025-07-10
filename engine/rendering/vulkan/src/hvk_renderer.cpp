#include "hvk_renderer.h"
#include "hvk_graphics.h"

namespace hf
{
    void DestroySurface(VkRenderer* rn)
    {
        vkDestroySurfaceKHR(GRAPHICS_DATA.instance, rn->swapchain.surface, nullptr);
        rn->swapchain.surface = VK_NULL_HANDLE;
    }

    uvec2 GetReadyForRendering(VkRenderer* rn, VkRenderTexture** pTextures, uint32_t textureCount)
    {
        if (rn->targetSize.x == 0 || rn->targetSize.y == 0) return {};
        return AcquireNextImage(rn, pTextures, textureCount);
    }

    void StartFrame(VkRenderer* rn)
    {
        auto& frame = rn->frames[rn->currentFrame];
        frame.usedCommandCount = 0;
        BeginCommandBuffer(rn, rn->commandPool.buffers[rn->currentFrame]);
    }

    void EndFrame(VkRenderer* rn)
    {
        EndCommandBuffer(rn);
        SubmitCommands(rn);
        PresentSwapchain(rn);

        rn->currentFrame = (rn->currentFrame + 1) % rn->frames.size();
    }

    void RegisterFrameBufferChange(VkRenderer* rn, uvec2 newSize)
    {
        std::lock_guard lock(rn->frameSync);
        rn->targetSize = newSize;
        rn->frameBufferChanged = true;
    }

    void SetVSync(VkRenderer* rn, VsyncMode mode)
    {
        std::lock_guard lock(rn->frameSync);
        rn->vSyncMode = mode;
        rn->frameBufferChanged = true;
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
