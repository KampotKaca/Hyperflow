#include "hvk_graphics.h"
#include "hvk_framebuffer.h"

namespace hf
{
    VkFrameBuffer::VkFrameBuffer(VkImageView* pAttachments, uint32_t attachmentCount, RenderPass pass, const VkExtent2D& extent)
    {
        auto& rnPass = GetDrawPass(pass);
        this->extent = extent;
        VkFramebufferCreateInfo frameBufferInfo
        {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = rnPass.pass,
            .attachmentCount = attachmentCount,
            .pAttachments = pAttachments,
            .width = extent.width,
            .height = extent.height,
            .layers = 1,
        };

        VK_HANDLE_EXCEPT(vkCreateFramebuffer(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
            &frameBufferInfo, nullptr, &buffer));
    }

    VkFrameBuffer::~VkFrameBuffer()
    {
        if (buffer != VK_NULL_HANDLE)
        {
            vkDestroyFramebuffer(GRAPHICS_DATA.defaultDevice->logicalDevice.device, buffer, nullptr);
        }
    }
}