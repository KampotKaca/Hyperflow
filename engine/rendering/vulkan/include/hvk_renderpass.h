#ifndef HVK_RENDERPASS_H
#define HVK_RENDERPASS_H

#include "hvk_shared.h"
#include "hvk_texture.h"
#include "hvk_textureallocator.h"

namespace hf
{
    struct VkRendererPassTextureCollection
    {
        RenderPass pass;
        std::vector<VkTexture*> colorTextures{};
        std::vector<VkTexture*> depthTextures{};
        VkTextureAllocator* allocator{};
    };

    struct VkDrawPass
    {
        explicit VkDrawPass(const RenderPassDefinitionInfo& info);
        ~VkDrawPass();

        VkRenderPass pass{};
        VkClearValue clearValue{};

        std::vector<RenderSubpassAttachmentInfo> colorAttachments{};
        std::vector<RenderSubpassAttachmentInfo> depthAttachments{};
        uint32_t attachmentCount = 0;
        uint32_t inputAttachmentCount = 0;
        uint32_t multisamplingAttachmentCount = 0;
        bool hasDepthStencilAttachment = false;
    };

    bool IsValidRenderPass(RenderPass pass);
    const VkDrawPass& GetRenderPass(RenderPass pass);
}

#endif //HVK_RENDERPASS_H
