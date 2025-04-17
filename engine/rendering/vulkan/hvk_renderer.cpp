#include "exceptions/hgraphicsexception.h"

#include <hwindow.h>

#include "hrenderer.h"
#include "hvk_graphics.h"
#include "hinternal.h"

namespace hf::inter::rendering
{
    void StartFrame(Renderer* rn)
    {
        auto renderer = (VKRendererData*)rn->handle;

        WaitForFences(*GRAPHICS_DATA.defaultDevice, &renderer->isInFlight, 1, true);

        uint32_t imageIndex;
        vkAcquireNextImageKHR(GRAPHICS_DATA.defaultDevice->logicalDevice.device, renderer->swapchain.swapchain,
            UINT64_MAX, renderer->isImageAvailable, VK_NULL_HANDLE, &imageIndex);

        BeginCommandBuffer(renderer, renderer->commandPool.buffers[0]);
        BeginRenderPass(GRAPHICS_DATA.renderPass, renderer);
        SetViewportAndScissor(renderer, renderer->swapchain);
    }

    void EndFrame(Renderer* rn)
    {
        auto renderer = (VKRendererData*)rn->handle;
        EndRenderPass(renderer);
        EndCommandBuffer(renderer);
        SubmitCommands(renderer);
    }

    void Draw(Renderer* rn)
    {
        auto renderer = (VKRendererData*)rn->handle;
        vkCmdDraw(renderer->currentCommand, 3, 1, 0, 0);
    }
}
