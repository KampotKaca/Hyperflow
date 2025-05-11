#include "hvk_renderpass.h"

#include "hrenderer.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    struct DepthStencilFormatInfo
    {
        VkFormat format{};
        bool hasStencil{};
        bool hasDepth{};
    };

    static void SetOperations(VkAttachmentLoadOp& loadOp, VkAttachmentStoreOp& storeOp, LoadStoreOperationType type);
    static DepthStencilFormatInfo ChooseDepthStencilFormat(LoadStoreOperationType op, LoadStoreOperationType stencilOp, VkImageTiling tiling, VkFormatFeatureFlags features);
    inline bool CheckFormatSupport(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features);
    static void CreateImageViews(VkRenderPassTexture* pTextures, uint32_t count, VkFormat format, VkImageAspectFlags aspectFlags);

    VkRenderPassTexture::VkRenderPassTexture(uvec2 size, VkFormat format,
        VkImageTiling tiling, VkImageUsageFlags imageUsageFlags)
    {
        QueueFamilyIndices& familyIndices = GRAPHICS_DATA.defaultDevice->familyIndices;
        uint32_t queus[2] = { familyIndices.transferFamily.value(), familyIndices.graphicsFamily.value() };
        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;

        VkImageCreateInfo imageInfo
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = format,
            .extent = { size.x, size.y, 1 },
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = tiling,
            .usage = imageUsageFlags,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 2,
            .pQueueFamilyIndices = queus,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        VK_HANDLE_EXCEPT(vkCreateImage(device, &imageInfo, nullptr, &image));
        AllocateImage(BufferMemoryType::Static, image, &memory);
    }

    VkRenderPassTexture::~VkRenderPassTexture()
    {
        vmaDestroyImage(GRAPHICS_DATA.allocator, image, memory);
        if (view) vkDestroyImageView(GRAPHICS_DATA.defaultDevice->logicalDevice.device, view, nullptr);
    }

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

        // for (uint32_t i = 0; i < info.supportedRendererCount; i++)
        // {
        //     auto& rn = info.pSupportedRenderers[i];
        //     VkRendererPassInfo passInfo = {};
        //     passInfo.colorTextures.reserve(colorAttachmentCount);
        //     passInfo.depthTextures.reserve(depthAttachmentCount);
        // }

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
                        .samples = (VkSampleCountFlagBits)attachmentInfo.msaaCounter,
                    };

                    if (attachmentInfo.finalLayout == RenderPassLayoutType::PresentSrc)
                        attachment.format = VULKAN_API_COLOR_FORMAT;
                    else attachment.format = (VkFormat)attachmentInfo.format;

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

    DepthStencilFormatInfo ChooseDepthStencilFormat(LoadStoreOperationType op, LoadStoreOperationType stencilOp,
        VkImageTiling tiling, VkFormatFeatureFlags features)
    {
        uint32_t stScore = (op != LoadStoreOperationType::DontCareAndDontCare) << 0;
        stScore |= (stencilOp != LoadStoreOperationType::DontCareAndDontCare) << 1;

        switch (stScore)
        {
        case 1:

            for (uint32_t i = 0; i < NUM_DEPTH_FORMATS; i++)
            {
                auto format = (VkFormat)inter::rendering::DEPTH_FORMATS[i];
                if (CheckFormatSupport(format, tiling, features)) return
                {
                    .format = format,
                    .hasStencil = false,
                    .hasDepth = true
                };
            }

            throw GENERIC_EXCEPT("[Hyperflow]", "No suitable depth format found");
            break;
        case 2:

            for (uint32_t i = 0; i < NUM_STENCIL_FORMATS; i++)
            {
                auto format = (VkFormat)inter::rendering::STENCIL_FORMATS[i];
                if (CheckFormatSupport(format, tiling, features)) return
                {
                    .format = format,
                    .hasStencil = true,
                    .hasDepth = false
                };
            }

            throw GENERIC_EXCEPT("[Hyperflow]", "No suitable stencil format found");
            break;
        case 3:

            for (uint32_t i = 0; i < NUM_DEPTH_STENCIL_FORMATS; i++)
            {
                auto format = (VkFormat)inter::rendering::DEPTH_STENCIL_FORMATS[i];
                if (CheckFormatSupport(format, tiling, features)) return
                {
                    .format = format,
                    .hasStencil = true,
                    .hasDepth = true
                };
            }

            throw GENERIC_EXCEPT("[Hyperflow]", "No suitable depthStencil format found");
            break;
        default: throw GENERIC_EXCEPT("[Hyperflow]", "Unused renderPass depthStencil attachment!!!");
        }
    }

    bool CheckFormatSupport(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features)
    {
        auto& prop = GRAPHICS_DATA.defaultDevice->formatProps[(uint32_t)format];
        switch (tiling)
        {
        case VK_IMAGE_TILING_LINEAR:
            if ((prop.linearTilingFeatures & features) == features) return true;
            break;
        case VK_IMAGE_TILING_OPTIMAL:
            if ((prop.optimalTilingFeatures & features) == features) return true;
            break;
        default: throw GENERIC_EXCEPT("[Hyperflow]", "Unused tiling type");
        }
        return false;
    }

    void CreateImageViews(VkRenderPassTexture* pTextures, uint32_t count,
        VkFormat format, VkImageAspectFlags aspectFlags)
    {
        SubmitBufferToImageCopyOperations();
        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        for (uint32_t i = 0; i < count; i++)
        {
            auto& texture = pTextures[i];
            VkImageViewCreateInfo viewInfo
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = texture.image,
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = format,
                .subresourceRange =
                {
                    .aspectMask = aspectFlags,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                }
            };

            VK_HANDLE_EXCEPT(vkCreateImageView(device, &viewInfo, nullptr, &texture.view));
        }
    }
}