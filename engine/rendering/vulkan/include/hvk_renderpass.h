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
    };

    struct VkDrawPass
    {
        explicit VkDrawPass(const RenderPassDefinitionInfo& info);
        ~VkDrawPass();

        VkRenderPass pass{};

        std::vector<RenderSubpassColorAttachmentInfo> colorAttachments{};
        std::vector<RenderSubpassDepthAttachmentInfo> depthAttachments{};
        std::vector<VkClearValue> clearValues{};
        uint32_t attachmentCount = 0;
        uint32_t multisamplingAttachmentCount = 0;
        bool hasPresentationAttachment = false;
    };

    bool IsValidRenderPass(RenderPass pass);
    const VkDrawPass& GetRenderPass(RenderPass pass);
}

#endif //HVK_RENDERPASS_H
