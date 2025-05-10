#include "hvk_drawpass.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    void CreateRenderPassColorAttachment(std::vector<VkAttachmentDescription>& attachments,
                                         std::vector<VkAttachmentReference>& attachmentRefs);

    static void SetOperations(VkAttachmentLoadOp& loadOp, VkAttachmentStoreOp& storeOp, LoadStoreOperationType type);

    VkDrawPass::VkDrawPass(const RenderPassDefinitionInfo& info)
    {
        clearValue.depthStencil = { info.depth, info.stencil };
        clearValue.color =
            { info.clearColor.x, info.clearColor.y,
              info.clearColor.z, info.clearColor.w };

        std::vector<VkSubpassDependency> dependencies(info.dependencyCount);
        std::vector<VkSubpassDescription> subpasses(info.subpassCount);

        for (uint32_t i = 0; i < info.dependencyCount; i++)
        {
            auto& dependency = info.pDependencies[i];
            dependencies[i] =
            {
                .srcSubpass = dependency.src.subpassIndex,
                .dstSubpass = dependency.dst.subpassIndex,
                .srcStageMask = (VkPipelineStageFlags)dependency.src.stageMask,
                .dstStageMask = (VkPipelineStageFlags)dependency.dst.stageMask,
                .srcAccessMask = (VkAccessFlags)dependency.src.accessMask,
                .dstAccessMask = (VkAccessFlags)dependency.dst.accessMask,
                .dependencyFlags = (VkDependencyFlags)dependency.flags
            };
        }

        for (uint32_t i = 0; i < info.subpassCount; i++)
        {
            auto& subpassInfo = info.pSubpasses[i];
            attachmentCount += subpassInfo.attachmentCount;
            for (uint32_t j = 0; j < subpassInfo.attachmentCount; j++)
            {
                auto& attachmentInfo = subpassInfo.pAttachments[j];
                if (attachmentInfo.msaaCounter > 1)
                {
                    attachmentCount++;
                    multisamplingAttachmentCount++;
                }

                switch (attachmentInfo.type)
                {
                case RenderPassAttachmentType::Input: inputAttachmentCount++; break;
                case RenderPassAttachmentType::Color: colorAttachmentCount++; break;
                }
            }

            if (subpassInfo.depthAttachment)
            {
                attachmentCount++;
                depthAttachmentCount++;
            }
        }

        std::vector<VkAttachmentDescription> attachments(attachmentCount);
        std::vector<VkAttachmentReference> inputAttachmentRefs(inputAttachmentCount);
        std::vector<VkAttachmentReference> colorAttachmentRefs(colorAttachmentCount);
        std::vector<VkAttachmentReference> msaaAttachmentRefs(multisamplingAttachmentCount);
        std::vector<VkAttachmentReference> depthAttachmentRefs(depthAttachmentCount);

        {
            uint32_t attachmentIndex = 0;
            uint32_t inputIndex = 0;
            uint32_t colorIndex = 0;
            uint32_t multisampleIndex = 0;
            uint32_t depthIndex = 0;
            for (uint32_t i = 0; i < info.subpassCount; i++)
            {
                auto& subpassInfo = info.pSubpasses[i];
                uint32_t inputStart = inputIndex;
                uint32_t colorStart = colorIndex;
                uint32_t multisampleStart = multisampleIndex;
                for (uint32_t j = 0; j < subpassInfo.attachmentCount; j++)
                {
                    auto& attachmentInfo = subpassInfo.pAttachments[j];
                    auto& attachment = attachments[attachmentIndex];
                    attachment =
                    {
                        .flags = (VkAttachmentDescriptionFlags)attachmentInfo.usesSharedMemory,
                        .format = (VkFormat)attachmentInfo.format,
                        .samples = (VkSampleCountFlagBits)attachmentInfo.msaaCounter,
                    };

                    SetOperations(attachment.loadOp, attachment.storeOp, attachmentInfo.lsOperation);
                    SetOperations(attachment.stencilLoadOp, attachment.stencilStoreOp, attachmentInfo.lsStencilOperation);

                    VkAttachmentReference ref =
                    {
                        .attachment = attachmentIndex,
                        .layout = (VkImageLayout)attachmentInfo.layout
                    };

                    switch (attachmentInfo.type)
                    {
                    case RenderPassAttachmentType::Input:
                        inputAttachmentRefs[inputIndex] = ref;
                        inputIndex++;
                        break;
                    case RenderPassAttachmentType::Color:
                        colorAttachmentRefs[colorIndex] = ref;
                        colorIndex++;
                        break;
                    }

                    attachmentIndex++;

                    if (attachmentInfo.msaaCounter > 1)
                    {
                        attachment.initialLayout = (VkImageLayout)attachmentInfo.initialLayout;
                        attachment.finalLayout = (VkImageLayout)attachmentInfo.layout;

                        auto& msaaAttachment = attachments[attachmentIndex];
                        msaaAttachment = attachment;
                        msaaAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
                        msaaAttachment.finalLayout = (VkImageLayout)attachmentInfo.finalLayout;

                        msaaAttachmentRefs[multisampleIndex] =
                        {
                            .attachment = attachmentIndex,
                            .layout = (VkImageLayout)attachmentInfo.layout
                        };

                        multisampleIndex++;
                        attachmentIndex++;
                    }
                    else
                    {
                        attachment.initialLayout = (VkImageLayout)attachmentInfo.initialLayout;
                        attachment.finalLayout = (VkImageLayout)attachmentInfo.finalLayout;
                    }
                }

                auto& subpass = subpasses[i];
                subpass =
                {
                    .pipelineBindPoint = (VkPipelineBindPoint)subpassInfo.bindingType,
                };

                if ((inputIndex - inputStart) > 0)
                {
                    subpass.inputAttachmentCount = inputIndex - inputStart;
                    subpass.pInputAttachments = &inputAttachmentRefs[inputStart];
                }

                if ((colorIndex - colorStart) > 0)
                {
                    subpass.colorAttachmentCount = colorIndex - colorStart;
                    subpass.pColorAttachments = &colorAttachmentRefs[colorStart];
                }

                if ((multisampleIndex - multisampleStart) > 0)
                {
                    subpass.pResolveAttachments = &msaaAttachmentRefs[multisampleIndex];
                }

                if (subpassInfo.depthAttachment)
                {
                    auto& depthAttachment = attachments[attachmentIndex];
                    depthAttachment =
                    {
                        .flags = (VkAttachmentDescriptionFlags)subpassInfo.depthAttachment->usesSharedMemory,
                        .format = (VkFormat)subpassInfo.depthAttachment->format,
                        .samples = VK_SAMPLE_COUNT_1_BIT,
                    };

                    SetOperations(depthAttachment.loadOp, depthAttachment.storeOp, subpassInfo.depthAttachment->lsOperation);
                    SetOperations(depthAttachment.stencilLoadOp, depthAttachment.stencilStoreOp, subpassInfo.depthAttachment->lsStencilOperation);

                    depthAttachmentRefs[depthIndex] =
                    {
                        .attachment = attachmentIndex,
                        .layout = (VkImageLayout)subpassInfo.depthAttachment->layout
                    };

                    subpass.pDepthStencilAttachment = &depthAttachmentRefs[depthIndex];
                    attachmentIndex++;
                    depthIndex++;
                }
            }
        }

        VkRenderPassCreateInfo renderPassInfo
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = (uint32_t)attachments.size(),
            .pAttachments = attachments.data(),
            .subpassCount = (uint32_t)subpasses.size(),
            .pSubpasses = subpasses.data(),
            .dependencyCount = (uint32_t)dependencies.size(),
            .pDependencies = dependencies.data(),
        };

        VK_HANDLE_EXCEPT(vkCreateRenderPass(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
            &renderPassInfo, nullptr, &pass));
    }

    VkDrawPass::~VkDrawPass()
    {
        vkDestroyRenderPass(GRAPHICS_DATA.defaultDevice->logicalDevice.device, pass, nullptr);
    }

    bool IsValidDrawPass(RenderPass pass)
    {
        return pass > 0 && pass <= GRAPHICS_DATA.renderPasses.size();
    }

    const VkDrawPass& GetDrawPass(RenderPass pass)
    {
        if (!IsValidDrawPass(pass)) throw GENERIC_EXCEPT("[Hyperflow]", "Invalid draw pass");
        return GRAPHICS_DATA.renderPasses[pass - 1];
    }

    void BeginPass(VkRenderer* rn, RenderPass pass)
    {
        if (rn->currentPass) throw GENERIC_EXCEPT("[Hyperflow]", "Begin render pass called twice without ending previous one");
        auto& drawPass = GetDrawPass(pass);
        const auto frameBuffer = rn->swapchain.frameBuffers[rn->imageIndex];
        VkRenderPassBeginInfo renderPassInfo
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = drawPass.pass,
            .framebuffer = frameBuffer->buffer,
            .renderArea =
            {
                .offset = {0, 0},
                .extent = frameBuffer->extent,
            },
            .clearValueCount = 1,
            .pClearValues = &drawPass.clearValue
        };

        vkCmdBeginRenderPass(rn->currentCommand, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        rn->currentPass = drawPass.pass;
        UploadViewportAndScissor(rn);
    }

    void EndPass(VkRenderer* rn)
    {
        if (rn->currentPass != VK_NULL_HANDLE)
        {
            vkCmdEndRenderPass(rn->currentCommand);
            rn->currentPass = VK_NULL_HANDLE;
        }
        else throw GENERIC_EXCEPT("[Hyperflow]", "End render pass called without begin render pass");
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

    void SetOperations(VkAttachmentLoadOp& loadOp, VkAttachmentStoreOp& storeOp, LoadStoreOperationType type)
    {
        switch (type)
        {
        case LoadStoreOperationType::LoadAndStore:
            loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            break;
        case LoadStoreOperationType::ClearAndStore:
            loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            break;
        case LoadStoreOperationType::DontCareAndStore:
            loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            break;
        case LoadStoreOperationType::LoadAndDontCare:
            loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            break;
        case LoadStoreOperationType::ClearAndDontCare:
            loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            break;
        case LoadStoreOperationType::DontCareAndDontCare:
            loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            break;
        }
    }
}