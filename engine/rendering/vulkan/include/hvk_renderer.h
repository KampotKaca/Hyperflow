#ifndef HVK_RENDERER_H
#define HVK_RENDERER_H

#include "hvk_shared.h"
#include "hvk_graphics.h"

namespace hf
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
        CommandPool transferPool{};
        uvec2 targetSize{};

        VkCommandBuffer currentCommand{};
        VkRenderPass currentPass{};

        VkBuffer drawBuffers[VULKAN_API_MAX_NUM_DRAW_BUFFER]{};
        VkDeviceSize drawOffsets[VULKAN_API_MAX_NUM_DRAW_BUFFER]{};

        std::vector<VkFrame> frames{};
        uint32_t currentFrame = 0;
        uint32_t imageIndex{};
        bool frameBufferResized = false;
    };

    struct VkDrawInfo
    {
        const VKRenderer* renderer;
        const VkBuffer* pBuffers;
        const VkDeviceSize* pOffsets;
        uint32_t bufferCount;
        uint32_t vertCount;
        uint32_t instanceCount;
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
    uint32_t GetMemoryType(uint32_t filter, VkMemoryPropertyFlags props);

    bool GetReadyForRendering(VKRenderer* rn);
    void StartFrame(VKRenderer* rn);
    void EndFrame(VKRenderer* rn);
    void RegisterFrameBufferChange(VKRenderer* rn, uvec2 newSize);
    void Draw(const VkDrawInfo& info);
}

#endif //HVK_RENDERER_H
