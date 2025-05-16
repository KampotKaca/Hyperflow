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

    VkDrawPass::VkDrawPass(const RenderPassDefinitionInfo& info)
    {
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

        uint32_t colorAttachmentCount = 0;
        uint32_t depthAttachmentCount = 0;

        for (uint32_t i = 0; i < info.subpassCount; i++)
        {
            auto& subpassInfo = info.pSubpasses[i];
            attachmentCount += subpassInfo.attachmentCount;
            for (uint32_t j = 0; j < subpassInfo.attachmentCount; j++)
            {
                auto& attachmentInfo = subpassInfo.pColorAttachments[j];
                if (attachmentInfo.msaaCounter > 1)
                {
                    attachmentCount++;
                    multisamplingAttachmentCount++;
                }
                colorAttachmentCount++;
            }

            if (subpassInfo.presentationAttachment)
            {
                hasPresentationAttachment = true;
                attachmentCount++;
                colorAttachmentCount++;
            }

            if (subpassInfo.depthAttachment)
            {
                attachmentCount++;
                depthAttachmentCount++;
            }
        }

        std::vector<VkAttachmentDescription> attachments(attachmentCount);
        std::vector<VkAttachmentReference> colorAttachmentRefs(colorAttachmentCount);
        std::vector<VkAttachmentReference> msaaAttachmentRefs(multisamplingAttachmentCount);
        std::vector<VkAttachmentReference> depthAttachmentRefs(depthAttachmentCount);

        uint32_t cCount = colorAttachmentCount;
        if (hasPresentationAttachment) cCount--;
        clearValues = std::vector<VkClearValue>(attachmentCount);
        colorAttachments.reserve(cCount);
        depthAttachments.reserve(depthAttachmentCount);

        {
            uint32_t attachmentIndex = 0;
            uint32_t colorIndex = 0;
            uint32_t depthIndex = 0;
            uint32_t multisampleIndex = 0;
            for (uint32_t i = 0; i < info.subpassCount; i++)
            {
                auto& subpassInfo = info.pSubpasses[i];
                uint32_t colorStart = colorIndex;
                uint32_t multisampleStart = multisampleIndex;
                for (uint32_t j = 0; j < subpassInfo.attachmentCount; j++)
                {
                    auto& attachmentInfo = subpassInfo.pColorAttachments[j];
                    auto& attachment = attachments[attachmentIndex];

                    clearValues[attachmentIndex] =
                    {
                        .color =
                        {
                            attachmentInfo.clearColor.x,
                            attachmentInfo.clearColor.y,
                            attachmentInfo.clearColor.z,
                            attachmentInfo.clearColor.w
                        }
                    };
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

                    colorAttachmentRefs[colorIndex] = ref;
                    colorIndex++;
                    colorAttachments.push_back(attachmentInfo);
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

                if (subpassInfo.presentationAttachment)
                {
                    auto& attachmentInfo = *subpassInfo.presentationAttachment;
                    auto& attachment = attachments[attachmentIndex];

                    clearValues[attachmentIndex] =
                    {
                        .color =
                        {
                            attachmentInfo.clearColor.x,
                            attachmentInfo.clearColor.y,
                            attachmentInfo.clearColor.z,
                            attachmentInfo.clearColor.w
                        }
                    };
                    attachment =
                    {
                        .flags = (VkAttachmentDescriptionFlags)attachmentInfo.usesSharedMemory,
                        .format = VULKAN_API_COLOR_FORMAT,
                        .samples = (VkSampleCountFlagBits)attachmentInfo.msaaCounter,
                    };

                    SetOperations(attachment.loadOp, attachment.storeOp, attachmentInfo.lsOperation);
                    SetOperations(attachment.stencilLoadOp, attachment.stencilStoreOp, attachmentInfo.lsStencilOperation);

                    VkAttachmentReference ref =
                    {
                        .attachment = attachmentIndex,
                        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                    };

                    colorAttachmentRefs[colorIndex] = ref;
                    colorIndex++;
                    attachmentIndex++;

                    if (attachmentInfo.msaaCounter > 1)
                    {
                        attachment.initialLayout = (VkImageLayout)attachmentInfo.initialLayout;
                        attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                        auto& msaaAttachment = attachments[attachmentIndex];
                        msaaAttachment = attachment;
                        msaaAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
                        msaaAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

                        msaaAttachmentRefs[multisampleIndex] =
                        {
                            .attachment = attachmentIndex,
                            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                        };

                        multisampleIndex++;
                        attachmentIndex++;
                    }
                    else
                    {
                        attachment.initialLayout = (VkImageLayout)attachmentInfo.initialLayout;
                        attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                    }
                }

                if (subpassInfo.depthAttachment)
                {
                    auto& depthAttachment = attachments[attachmentIndex];
                    depthAttachments.push_back(*subpassInfo.depthAttachment);
                    auto& da = *subpassInfo.depthAttachment;

                    clearValues[attachmentIndex] =
                    {
                        .depthStencil = { da.clearDepth, da.clearStencil }
                    };

                    depthAttachment =
                    {
                        .flags = (VkAttachmentDescriptionFlags)da.usesSharedMemory,
                        .samples = VK_SAMPLE_COUNT_1_BIT,
                        .initialLayout = (VkImageLayout)da.initialLayout,
                        .finalLayout = (VkImageLayout)da.finalLayout
                    };

                    SetOperations(depthAttachment.loadOp, depthAttachment.storeOp, subpassInfo.depthAttachment->lsOperation);
                    SetOperations(depthAttachment.stencilLoadOp, depthAttachment.stencilStoreOp, subpassInfo.depthAttachment->lsStencilOperation);

                    auto formatInfo = ChooseDepthStencilFormat(da.lsOperation, da.lsStencilOperation,
                        VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
                    depthAttachment.format = formatInfo.format;
                    depthAttachmentRefs[depthIndex] =
                    {
                        .attachment = attachmentIndex,
                        .layout = (VkImageLayout)subpassInfo.depthAttachment->layout
                    };

                    subpass.pDepthStencilAttachment = &depthAttachmentRefs[depthIndex];
                    attachmentIndex++;
                    depthIndex++;
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

    bool IsValidRenderPass(RenderPass pass)
    {
        return pass > 0 && pass <= GRAPHICS_DATA.renderPasses.size();
    }

    const VkDrawPass& GetRenderPass(RenderPass pass)
    {
        if (!IsValidRenderPass(pass)) throw GENERIC_EXCEPT("[Hyperflow]", "Invalid draw pass");
        return GRAPHICS_DATA.renderPasses[pass - 1];
    }

    void BeginPass(VkRenderer* rn, RenderPass pass)
    {
        if (rn->currentPass) throw GENERIC_EXCEPT("[Hyperflow]", "Begin render pass called twice without ending previous one");
        auto& drawPass = GetRenderPass(pass);
        const auto frameBuffer = rn->swapchain.images[rn->imageIndex].frameBuffer;
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
            .clearValueCount = (uint32_t)drawPass.clearValues.size(),
            .pClearValues = drawPass.clearValues.data()
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
                    .hasStencil = format != VK_FORMAT_D32_SFLOAT && format != VK_FORMAT_D16_UNORM,
                    .hasDepth = true
                };
            }

            throw GENERIC_EXCEPT("[Hyperflow]", "No suitable depth format found");
        case 2:

            for (uint32_t i = 0; i < NUM_STENCIL_FORMATS; i++)
            {
                auto format = (VkFormat)inter::rendering::STENCIL_FORMATS[i];
                if (CheckFormatSupport(format, tiling, features)) return
                {
                    .format = format,
                    .hasStencil = true,
                    .hasDepth = format != VK_FORMAT_S8_UINT
                };
            }

            throw GENERIC_EXCEPT("[Hyperflow]", "No suitable stencil format found");
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

    void BindPassToRenderer(VkRenderer* rn, RenderPass pass, uvec2 size)
    {
        auto& renderPass = GetRenderPass(pass);
        VkRendererPassTextureCollection texCollection
        {
            .pass = pass,
        };
        // texCollection.colorTextures.reserve(renderPass.colorAttachments.size());
        texCollection.depthTextures = std::vector<VkTexture*>(renderPass.depthAttachments.size());

        // for (uint32_t i = 0; i < renderPass.colorAttachments.size(); i++)
        // {
        //     auto tex = VkRenderPassTexture(rn->targetSize, );
        //     texCollection.colorTextures.push_back(std::move(tex));
        // }

        for (uint32_t i = 0; i < renderPass.depthAttachments.size(); i++)
        {
            auto& attachment = renderPass.depthAttachments[i];
            auto supportedDepthFormat = ChooseDepthStencilFormat(attachment.lsOperation, attachment.lsStencilOperation,
                VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

            inter::rendering::TextureCreationInfo textureInfo
            {
                .size = uvec3(size, 1),
                .channel = TextureChannel::RGBA,
                .mipLevels = 1,
                .data = nullptr,
                .details
                {
                    .type = TextureType::Tex2D,
                    .format = (TextureFormat)supportedDepthFormat.format,
                    .tiling = TextureTiling::Optimal,
                    .usage = TextureUsageFlags::DepthStencil,
                    .memoryType = BufferMemoryType::Static,
                    .finalLayout = attachment.finalLayout,
                }
            };

            textureInfo.details.aspectFlags = TextureAspectFlags::None;
            if (supportedDepthFormat.hasDepth) textureInfo.details.aspectFlags |= TextureAspectFlags::Depth;
            if (supportedDepthFormat.hasStencil) textureInfo.details.aspectFlags |= TextureAspectFlags::Stencil;

            auto* texture = new VkTexture(textureInfo);
            texCollection.depthTextures[i] = texture;
        }

        rn->passTextureCollections.push_back(std::move(texCollection));
    }

    void RebindRendererToAllPasses(VkRenderer* rn)
    {
        std::vector<RenderPass> passes(rn->passTextureCollections.size());
        for (uint32_t i = 0; i < rn->passTextureCollections.size(); i++)
            passes[i] = rn->passTextureCollections[i].pass;

        ClearRendererPassData(rn);
        for (uint32_t i = 0; i < passes.size(); i++) BindPassToRenderer(rn, passes[i],
            uvec2(rn->viewport.width, rn->viewport.height));
    }

    void ClearRendererPassData(VkRenderer* rn)
    {
        for (auto& texCollection : rn->passTextureCollections)
        {
            for (auto& tex : texCollection.colorTextures) delete tex;
            for (auto& tex : texCollection.depthTextures) delete tex;
        }
        rn->passTextureCollections.clear();
    }
}