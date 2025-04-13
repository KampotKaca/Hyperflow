#include "hvk_graphics.h"
#include "exceptions/hgraphicsexception.h"

namespace hf::inter::rendering
{
    void CreateRenderPass(VkRenderPass* renderPass)
    {
        VkAttachmentDescription colorAttachment
        {
            .format = VK_FORMAT_B8G8R8A8_SRGB,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        };

        VkAttachmentReference colorAttachmentRef
        {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };

        VkSubpassDescription subpass
        {
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachmentRef
        };

        VkRenderPassCreateInfo renderPassInfo
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = 1,
            .pAttachments = &colorAttachment,
            .subpassCount = 1,
            .pSubpasses = &subpass,
        };

        VK_HANDLE_EXCEPT(vkCreateRenderPass(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
            &renderPassInfo, nullptr, renderPass));
    }

    void DestroyRenderPass(const VkRenderPass& renderPass)
    {
        vkDestroyRenderPass(GRAPHICS_DATA.defaultDevice->logicalDevice.device, renderPass, nullptr);
    }
}