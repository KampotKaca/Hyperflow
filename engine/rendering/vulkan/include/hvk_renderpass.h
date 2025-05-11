#ifndef HVK_RENDERPASS_H
#define HVK_RENDERPASS_H

#include "hvk_shared.h"

namespace hf
{
    struct VkRenderPassTexture
    {
        VkRenderPassTexture(uvec2 size, VkFormat format, VkImageTiling tiling,
            VkImageUsageFlags imageUsageFlags);
        ~VkRenderPassTexture();
        VkImage image{};
        VkImageView view{};
        VmaAllocation memory{};
    };

    struct VkRendererPassInfo
    {
        std::vector<VkRenderPassTexture> colorTextures{};
        std::vector<VkRenderPassTexture> depthTextures{};
    };

    struct VkDrawPass
    {
        VkDrawPass(const RenderPassDefinitionInfo& info);
        ~VkDrawPass();

        VkRenderPass pass{};
        VkClearValue clearValue{};

        uint32_t attachmentCount = 0;
        uint32_t inputAttachmentCount = 0;
        uint32_t colorAttachmentCount = 0;
        uint32_t multisamplingAttachmentCount = 0;
        uint32_t depthAttachmentCount = 0;
        bool hasDepthStencilAttachment = false;
    };

    bool IsValidDrawPass(RenderPass pass);
    const VkDrawPass& GetDrawPass(RenderPass pass);
}

#endif //HVK_RENDERPASS_H
