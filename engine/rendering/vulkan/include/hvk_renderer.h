#ifndef HVK_RENDERER_H
#define HVK_RENDERER_H

#include "hvk_shared.h"
#include "hvk_graphics.h"
#include "hvk_renderpass.h"

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
        VkRenderPass currentPass{};

        VkBuffer vertBufferCache[MAX_NUM_BUFFER_CACHE]{};
        VkDeviceSize drawOffsets[MAX_NUM_BUFFER_CACHE]{};

        std::vector<VkRendererPassTextureCollection> passTextureCollections{};

        VkPipelineLayout currentLayout{};
        RenderPass mainPass{};

        std::vector<VkFrame> frames{};
        uint32_t currentFrame = 0;
        uint32_t imageIndex = UINT32_MAX;
        VsyncMode vSyncMode = VsyncMode::Relaxed;
        bool frameBufferResized = false;
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

    void PostRendererLoad(VkRenderer* rn, RenderPass pass);

    void BindPassToRenderer(VkRenderer* rn, RenderPass pass, uvec2 size);
    void RebindRendererToAllPasses(VkRenderer* rn);
    void ClearRendererPassData(VkRenderer* rn);

    void DestroySurface(VkRenderer* rn);
    void SetupViewportAndScissor(VkRenderer* rn);

    void PresentSwapchain(VkRenderer* rn);
    bool AcquireNextImage(VkRenderer* rn);
    void DelayUntilPreviousFrameFinish(VkRenderer* rn);

    void BeginPass(VkRenderer* rn, RenderPass pass);
    void EndPass(VkRenderer* rn);

    void BeginCommandBuffer(VkRenderer* rn, VkCommandBuffer buffer);
    void EndCommandBuffer(VkRenderer* rn);
    void SubmitCommands(VkRenderer* rn);

    void CreateSwapchainFrameBuffers(VkRenderer* rn);
    void DestroySwapchainFrameBuffers(VkRenderer* rn);
    void UploadViewportAndScissor(const VkRenderer* rn);

    bool GetReadyForRendering(VkRenderer* rn);
    void StartFrame(VkRenderer* rn);
    void EndFrame(VkRenderer* rn);
    void Draw(const VkDrawInfo& info);

    void RegisterFrameBufferChange(VkRenderer* rn, uvec2 newSize);
    void SetVSync(VkRenderer* rn, VsyncMode mode);

    void UploadUniforms(const VkRenderer* rn, const inter::rendering::UniformBufferUploadInfo& info);
    void BindUniforms(const VkRenderer* rn, const UniformBufferBindInfo& info);
    void BindTexturePack(VkRenderer* rn, VkTexturePack* pack);
    void BindShaderSetup(VkRenderer* rn, ShaderSetup setup);
    void UploadPushConstants(const VkRenderer* rn, const PushConstantUploadInfo& info);
}

#endif //HVK_RENDERER_H
