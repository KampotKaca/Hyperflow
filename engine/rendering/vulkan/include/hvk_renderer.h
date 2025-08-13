#ifndef HVK_RENDERER_H
#define HVK_RENDERER_H

#include "hvk_shared.h"
#include "hvk_graphics.h"
#include "hvk_indexbuffer.h"
#include "hvk_rendertexture.h"

namespace hf
{
    struct VkRenderer
    {
        explicit VkRenderer(const inter::rendering::RendererInstanceCreationInfo_i& info);
        ~VkRenderer();

        void (*shutdownCallback)();
        void* windowHandle = nullptr;
        GraphicsSwapChain swapchain{};
        VkViewport viewport{};
        VkRect2D scissor{};
        CommandPool commandPool{};
        uvec2 targetSize{};

        VkCommandBuffer currentCommand{};
        VkPipelineLayout currentLayout{};

        std::vector<VkFrame> frames{};
        uint32_t currentFrame = 0;
        uint32_t imageIndex = UINT32_MAX;
        VsyncMode vSyncMode = VsyncMode::Relaxed;
        volatile bool frameBufferChanged = false;
        std::mutex frameSync{};

        VkRenderTexture* prevRenderTexture{};
        VkRenderTexture* currentRenderTexture{};
    };

    void DestroySurface(VkRenderer* rn);
    void SetupViewportAndScissor(VkRenderer* rn);

    void PresentSwapchain(const VkRenderer* rn);
    uvec2 AcquireNextImage(VkRenderer* rn, VkRenderTexture** pTextures, uint32_t textureCount);
    void DelayUntilPreviousFrameFinish(VkRenderer* rn);

    void BeginRendering(const VkRenderer* rn);
    void EndRendering(const VkRenderer* rn);

    void BeginCommandBuffer(VkRenderer* rn, VkCommandBuffer buffer);
    void EndCommandBuffer(VkRenderer* rn);
    void SubmitCommands(VkRenderer* rn);

    void UploadViewportAndScissor(const VkRenderer* rn);

    void UploadBuffer(const VkRenderer* rn, const VkVertexBuffer* buffer, const void* data, uint32_t offsetInBytes, uint32_t sizeInBytes);
    void UploadBuffer(const VkRenderer* rn, const VkIndexBuffer* buffer, const void* data, uint32_t offset, uint32_t indexCount);

    uvec2 GetReadyForRendering(VkRenderer* rn, VkRenderTexture** pTextures, uint32_t textureCount);
    void StartFrame(VkRenderer* rn);
    void EndFrame(VkRenderer* rn);
    void Draw(const VkRenderer* rn, const inter::rendering::IndexedDrawCallInfo_i& info);
    void Draw(const VkRenderer* rn, const inter::rendering::VertexedDrawCallInfo_i& info);

    void RegisterFrameBufferChange(VkRenderer* rn, uvec2 newSize);
    void SetVSync(VkRenderer* rn, VsyncMode mode);

    void UploadBuffers(const VkRenderer* rn, const inter::rendering::BufferUploadInfo_i& info);
    void BindBuffer(const VkRenderer* rn, const inter::rendering::BindResourceInfo_i<Buffer>& info);
    void BindTexturePack(const VkRenderer* rn, const inter::rendering::BindResourceInfo_i<VkTexturePack*>& info);

    void BindShaderLayout(VkRenderer* rn, ShaderLayout setup);
    void UploadPushConstants(const VkRenderer* rn, const PushConstantUploadInfo& info);

    void FinishObjectBinding(const VkRenderer* rn, VkPipelineBindPoint bindPoint, uint32_t setBindingIndex);
}

#endif //HVK_RENDERER_H
