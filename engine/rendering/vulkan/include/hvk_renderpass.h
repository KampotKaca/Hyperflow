#ifndef HVK_RENDERPASS_H
#define HVK_RENDERPASS_H

#include "hvk_shared.h"

namespace hf
{
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
