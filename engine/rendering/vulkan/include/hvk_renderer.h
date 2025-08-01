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

        VkBuffer vertBufferCache[MAX_NUM_BUFFER_CACHE]{};
        VkDeviceSize drawOffsets[MAX_NUM_BUFFER_CACHE]{};

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

    void PresentSwapchain(const VkRenderer* rn);
    uvec2 AcquireNextImage(VkRenderer* rn, VkRenderTexture** pTextures, uint32_t textureCount);
    void DelayUntilPreviousFrameFinish(VkRenderer* rn);

    void BeginRendering(const VkRenderer* rn);
    void EndRendering(const VkRenderer* rn);

    void BeginCommandBuffer(VkRenderer* rn, VkCommandBuffer buffer);
    void EndCommandBuffer(VkRenderer* rn);
    void SubmitCommands(VkRenderer* rn);

    void UploadViewportAndScissor(const VkRenderer* rn);

    void UploadBuffer(const VkRenderer* rn, const VkVertBuffer* buffer, const void* data, uint32_t offset, uint32_t vertexCount);
    void UploadBuffer(const VkRenderer* rn, const VkIndexBuffer* buffer, const void* data, uint32_t offset, uint32_t indexCount);

    uvec2 GetReadyForRendering(VkRenderer* rn, VkRenderTexture** pTextures, uint32_t textureCount);
    void StartFrame(VkRenderer* rn);
    void EndFrame(VkRenderer* rn);
    void Draw(const VkDrawInfo& info);

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
