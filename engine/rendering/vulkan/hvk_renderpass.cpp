#include "include/hvk_graphics.h"
#include "include/hvk_renderer.h"

namespace hf
{
    void CreateRenderPassColorAttachment(std::vector<VkAttachmentDescription>& attachments,
                                         std::vector<VkAttachmentReference>& attachmentRefs);

    void CreateRenderPass(const VkRenderPassCreationInfo& info, VkRenderPass* renderPass)
    {
        VkSubpassDependency dependency
        {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        };

        std::vector<VkAttachmentDescription> attachments;
        std::vector<VkAttachmentReference> attachmentRefs;

        for (uint32_t i = 0; i < info.attachmentCount; i++)
        {
            switch (info.pAttachments[i])
            {
                case VkRenderPassAttachmentType::Color: CreateRenderPassColorAttachment(attachments, attachmentRefs); break;
                default: throw GENERIC_EXCEPT("[Hyperflow]", "Unsupported type of render pass attachment.");
            }
        }

        VkSubpassDescription subpass
        {
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = (uint32_t)attachmentRefs.size(),
            .pColorAttachments = attachmentRefs.data()
        };

        VkRenderPassCreateInfo renderPassInfo
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = (uint32_t)attachments.size(),
            .pAttachments = attachments.data(),
            .subpassCount = 1,
            .pSubpasses = &subpass,
            .dependencyCount = 1,
            .pDependencies = &dependency,
        };

        VK_HANDLE_EXCEPT(vkCreateRenderPass(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
            &renderPassInfo, nullptr, renderPass));
    }

    void DestroyRenderPass(VkRenderPass* renderPass)
    {
        if (*renderPass != VK_NULL_HANDLE)
        {
            vkDestroyRenderPass(GRAPHICS_DATA.defaultDevice->logicalDevice.device, *renderPass, nullptr);
            *renderPass = VK_NULL_HANDLE;
        }
    }

    void BeginRenderPass(VKRenderer* rn, const VkRenderPass& renderPass)
    {
        VkClearValue clearColor =
        {
            .color = { 0.0f, 0.0f, 0.0f, 1.0f },
        };

        const auto frameBuffer = rn->swapchain.frameBuffers[rn->imageIndex];
        VkRenderPassBeginInfo renderPassInfo
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = renderPass,
            .framebuffer = frameBuffer->buffer,
            .renderArea =
            {
                .offset = {0, 0},
                .extent = frameBuffer->extent,
            },
            .clearValueCount = 1,
            .pClearValues = &clearColor
        };

        vkCmdBeginRenderPass(rn->currentCommand, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        rn->currentPass = renderPass;
    }

    void EndRenderPass(VKRenderer* rn)
    {
        if (rn->currentPass != VK_NULL_HANDLE)
        {
            vkCmdEndRenderPass(rn->currentCommand);
            rn->currentPass = VK_NULL_HANDLE;
        }
    }

    void CreateRenderPassColorAttachment(std::vector<VkAttachmentDescription>& attachments,
                                         std::vector<VkAttachmentReference>& attachmentRefs)
    {
        uint32_t attachmentIndex = attachments.size();
        VkAttachmentDescription colorAttachment
        {
            .format = VULKAN_API_COLOR_FORMAT,
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
            .attachment = attachmentIndex,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };

        attachments.push_back(colorAttachment);
        attachmentRefs.push_back(colorAttachmentRef);
    }
}