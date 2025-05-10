#ifndef HVK_FRAMEBUFFER_H
#define HVK_FRAMEBUFFER_H

#include "hvk_shared.h"

namespace hf
{
    struct VkFrameBuffer
    {
        VkFrameBuffer() = default;
        VkFrameBuffer(VkImageView* pAttachments, RenderPass pass, const VkExtent2D& extent);
        ~VkFrameBuffer();

        VkFramebuffer buffer{};
        VkExtent2D extent{};
    };
}

#endif //HVK_FRAMEBUFFER_H
