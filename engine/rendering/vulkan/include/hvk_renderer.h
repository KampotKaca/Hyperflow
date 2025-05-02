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
        VKRenderer* renderer{};
        VkBuffer* pBuffers{};
        VkDeviceSize* pOffsets{};

        VkBuffer indexBuffer{};
        VkIndexType indexType{};
        uint32_t indexCount{};

        uint32_t bufferCount{};
        uint32_t vertCount{};
        uint32_t instanceCount{};
    };
    
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

    bool GetReadyForRendering(VKRenderer* rn);
    void StartFrame(VKRenderer* rn);
    void EndFrame(VKRenderer* rn);
    void RegisterFrameBufferChange(VKRenderer* rn, uvec2 newSize);
    void Draw(const VkDrawInfo& info);

    void UploadUniforms(const VKRenderer* rn, const UniformBufferUploadInfo& info);
    void BindUniformStorage(VKRenderer* rn, UniformStorage storage);
}

#endif //HVK_RENDERER_H
