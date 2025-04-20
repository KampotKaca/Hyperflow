#ifndef HVK_RENDERER_H
#define HVK_RENDERER_H

#include "hvk_graphics.h"

namespace hf::inter::rendering
{
    struct VKRenderer
    {
        VKRenderer(void* handle, uvec2 size);
        ~VKRenderer();

        void* windowHandle = nullptr;
        GraphicsSwapChain swapchain{};
        VkViewport viewport{};
        VkRect2D scissor{};
        CommandPool commandPool{};
        uvec2 targetSize{};
        SwapChainSupportDetails swapchainSupport{};

        VkCommandBuffer currentCommand{};
        VkRenderPass currentPass{};

        std::vector<VkFrame> frames{};
        uint32_t currentFrame = 0;
        uint32_t imageIndex{};
        bool frameBufferResized = false;
    };

    void CreateSurface(VKRenderer* rn);
    void DestroySurface(VKRenderer* rn);

    void SetupViewportAndScissor(VKRenderer* rn);

    void PresentSwapchain(VKRenderer* rn);
    bool AcquireNextImage(VKRenderer* rn);

    void BeginRenderPass(VKRenderer* rn, const VkRenderPass& renderPass);
    void EndRenderPass(VKRenderer* rn);

    void BeginCommandBuffer(VKRenderer* rn, VkCommandBuffer buffer);
    void EndCommandBuffer(VKRenderer* rn);
    void SubmitCommands(VKRenderer* rn);

    void CreateRendererFrameBuffers(VKRenderer* rn);
    void DestroyRendererFrameBuffers(VKRenderer* rn);

    void UploadViewportAndScissor(const VKRenderer* rn);
}

#endif //HVK_RENDERER_H
