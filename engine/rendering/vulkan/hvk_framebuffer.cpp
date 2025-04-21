#include "include/hvk_graphics.h"
#include "include/hvk_framebuffer.h"
#include "exceptions/hgraphicsexception.h"

namespace hf::inter::rendering
{
    VkFrameBuffer::VkFrameBuffer(VkImageView* pAttachments, uint32_t attachmentCount,
        VkRenderPass rnPass, const VkExtent2D& extent)
    {
        this->extent = extent;
        VkFramebufferCreateInfo frameBufferInfo
        {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = rnPass,
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