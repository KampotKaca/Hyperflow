#ifndef HVK_RENDERER_H
#define HVK_RENDERER_H

#include "hvk_shared.h"
#include "hvk_graphics.h"

namespace hf
{
    struct VkRenderer
    {
        VkRenderer(void* handle, uvec2 size);
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

        VkPipelineLayout currentLayout;

        std::vector<VkFrame> frames{};
        uint32_t currentFrame = 0;
        uint32_t imageIndex{};
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
    
    void DestroySurface(VkRenderer* rn);

    void SetupViewportAndScissor(VkRenderer* rn);

    void PresentSwapchain(VkRenderer* rn);
    bool AcquireNextImage(VkRenderer* rn);

    void BeginPass(VkRenderer* rn, RenderPass pass);
    void EndPass(VkRenderer* rn);

    void BeginCommandBuffer(VkRenderer* rn, VkCommandBuffer buffer);
    void EndCommandBuffer(VkRenderer* rn);
    void SubmitCommands(VkRenderer* rn);

    void CreateRendererFrameBuffers(VkRenderer* rn);
    void DestroyRendererFrameBuffers(VkRenderer* rn);
    void UploadViewportAndScissor(const VkRenderer* rn);

    bool GetReadyForRendering(VkRenderer* rn);
    void StartFrame(VkRenderer* rn);
    void EndFrame(VkRenderer* rn);
    void RegisterFrameBufferChange(VkRenderer* rn, uvec2 newSize);
    void Draw(const VkDrawInfo& info);

    void UploadUniforms(const VkRenderer* rn, const UniformBufferUploadInfo& info);
    void BindTexturePack(VkRenderer* rn, VkTexturePack* pack);
    void BindShaderSetup(VkRenderer* rn, ShaderSetup setup);
}

#endif //HVK_RENDERER_H
