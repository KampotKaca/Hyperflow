#ifndef HVK_RENDERER_H
#define HVK_RENDERER_H

#include "hvk_shared.h"
#include "hvk_graphics.h"
#include "hvk_rendertexture.h"

namespace hf
{
    struct VkRenderer
    {
        explicit VkRenderer(const inter::rendering::RendererInstanceCreationInfo& info);
        ~VkRenderer();

        void* windowHandle = nullptr;
        GraphicsSwapChain swapchain{};
        VkViewport viewport{};
        VkRect2D scissor{};
        CommandPool commandPool{};
        uvec2 targetSize{};

        VkCommandBuffer currentCommand{};

        VkBuffer vertBufferCache[MAX_NUM_BUFFER_CACHE]{};
        VkDeviceSize drawOffsets[MAX_NUM_BUFFER_CACHE]{};

        VkPipelineLayout currentLayout{};

        VkRenderTexture* renderTex{};
        std::vector<VkFrame> frames{};
        uint32_t currentFrame = 0;
        uint32_t imageIndex = UINT32_MAX;
        VsyncMode vSyncMode = VsyncMode::Relaxed;
        volatile bool frameBufferChanged = false;
        std::mutex frameSync{};
    };

    struct VkDrawInfo
    {
        VkRenderer* renderer{};
        VkBuffer* pBuffers{};
        VkDeviceSize* pOffsets{};

        VkBuffer indexBuffer{};
        VkIndexType indexType{};
        uint32_t indexCount{};

        uint32_t bufferCount{};
        uint32_t vertCount{};
        uint32_t instanceCount{};
    };

    void DestroySurface(VkRenderer* rn);
    void SetupViewportAndScissor(VkRenderer* rn);

    void PresentSwapchain(VkRenderer* rn);
    bool AcquireNextImage(VkRenderer* rn);
    void DelayUntilPreviousFrameFinish(VkRenderer* rn);

    void BeginRendering(const VkRenderer* rn, VkRenderTexture* tex);
    void EndRendering(VkRenderer* rn, VkRenderTexture* tex);

    void BeginCommandBuffer(VkRenderer* rn, VkCommandBuffer buffer);
    void EndCommandBuffer(VkRenderer* rn);
    void SubmitCommands(VkRenderer* rn);

    void UploadViewportAndScissor(const VkRenderer* rn);

    bool GetReadyForRendering(VkRenderer* rn);
    void StartFrame(VkRenderer* rn);
    void EndFrame(VkRenderer* rn);
    void Draw(const VkDrawInfo& info);

    void RegisterFrameBufferChange(VkRenderer* rn, uvec2 newSize);
    void AttachRenderTexture(VkRenderer* rn, VkRenderTexture* tex);

    void SetVSync(VkRenderer* rn, VsyncMode mode);

    void UploadBuffers(const VkRenderer* rn, const inter::rendering::BufferUploadInfo& info);
    void BindBuffers(const VkRenderer* rn, const inter::rendering::BufferBindInfo& info);
    void BindTexturePack(const VkRenderer* rn, const VkTexturePack* pack, uint32_t setBindingIndex);
    void BindShaderSetup(VkRenderer* rn, ShaderSetup setup);
    void UploadPushConstants(const VkRenderer* rn, const PushConstantUploadInfo& info);
}

#endif //HVK_RENDERER_H
