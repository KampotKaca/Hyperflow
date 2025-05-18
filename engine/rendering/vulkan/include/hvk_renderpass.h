#ifndef HVK_RENDERPASS_H
#define HVK_RENDERPASS_H

#include "hvk_shared.h"
#include "hvk_texture.h"

namespace hf
{
    struct VkRendererPassTextureCollection
    {
        RenderPass pass{};
        std::vector<VkTexture*> colorTextures{};
        std::vector<VkTexture*> depthTextures{};
        std::vector<VkTexture*> msaaTextures{};
    };

    struct VkDrawPass
    {
        explicit VkDrawPass(const RenderPassDefinitionInfo& info);
        ~VkDrawPass();

        VkRenderPass pass{};

        std::vector<VkAttachmentDescription> attachments{};
        std::vector<VkAttachmentReference> colorAttachmentRefs{};
        std::vector<VkAttachmentReference> depthAttachmentRefs{};
        std::vector<VkAttachmentReference> msaaAttachmentRefs{};
        std::vector<uint32_t> msaaSamples{};

        std::vector<VkClearValue> clearValues{};
        bool hasPresentationAttachment = false;
    };

    bool IsValidRenderPass(RenderPass pass);
    const VkDrawPass& GetRenderPass(RenderPass pass);
}

#endif //HVK_RENDERPASS_H
