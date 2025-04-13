#ifndef HVK_FRAMEBUFFER_H
#define HVK_FRAMEBUFFER_H

#include <vulkan/vulkan.h>
#include "hshared.h"

namespace hf::inter::rendering
{
    struct VkFrameBuffer
    {
        VkFrameBuffer() = default;
        VkFrameBuffer(VkImageView* pAttachments, uint32_t attachmentCount,
            VkRenderPass rnPass, const VkExtent2D& extent);
        ~VkFrameBuffer();

        VkFramebuffer buffer{};
        VkExtent2D extent{};
    };
}

#endif //HVK_FRAMEBUFFER_H
