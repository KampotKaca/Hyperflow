#include "hvk_rendertexture.h"

#include "hrenderer.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    struct DepthStencilFormatInfo
    {
        VkFormat format{};
        DepthStencilMode mode = DepthStencilMode::None;
    };

    static void SetOperations(VkAttachmentLoadOp& loadOp, VkAttachmentStoreOp& storeOp, LoadStoreOperationType type);
    static DepthStencilFormatInfo ChooseDepthStencilFormat(DepthStencilMode mode, VkImageTiling tiling, VkFormatFeatureFlags features);
    inline bool CheckFormatSupport(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features);

    VkRenderTexture::VkRenderTexture(const RenderTextureCreationInfo& info)
    : colorAttachmentCount(info.colorAttachmentCount), offset((int32_t)info.offset.x, (int32_t)info.offset.y), multisampleMode(info.multisampleMode)
    {
        for (uint32_t i = 0; i < info.colorAttachmentCount; i++)
        {
            auto& attachmentInfo = info.pColorAttachments[i];
            auto& attachment = colorAttachments[i];

            attachment =
            {
                .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
                .pNext = nullptr,
                .imageView = nullptr,
                .imageLayout = (VkImageLayout)attachmentInfo.layout,
                .resolveMode = VK_RESOLVE_MODE_NONE,
                .clearValue =
                {
                    attachmentInfo.clearColor.x,
                    attachmentInfo.clearColor.y,
                    attachmentInfo.clearColor.z,
                    attachmentInfo.clearColor.w
                }
            };

            SetOperations(attachment.loadOp, attachment.storeOp, attachmentInfo.lsOperation);
            colorInfos[i] =
            {
                .format = (VkFormat)attachmentInfo.format,
                .usage = (VkImageUsageFlags)attachmentInfo.usageFlags
            };

            if (attachmentInfo.isUsedForPresentation)
            {
                if (presentationAttachmentIndex != -1)
                    throw GENERIC_EXCEPT("[Hyperflow]", "Only one color attachment can be used for presentation");
                presentationAttachmentIndex = i;
            }
        }

        if (info.depthAttachment.has_value())
        {
            auto& attachmentInfo = info.depthAttachment.value();

            VkRenderingAttachmentInfoKHR attachment
            {
                .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
                .pNext = nullptr,
                .imageView = nullptr,
                .imageLayout = (VkImageLayout)attachmentInfo.layout,
                .clearValue =
                {
                    .depthStencil = { attachmentInfo.clearDepth, attachmentInfo.clearStencil }
                }
            };

            SetOperations(attachment.loadOp, attachment.storeOp, attachmentInfo.lsOperation);
            depthStencilAttachment = attachment;
            const auto formatInfo = ChooseDepthStencilFormat(attachmentInfo.mode,
                    VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
            depthStencilInfo =
            {
                .format = formatInfo.format,
                .usage = (VkImageUsageFlags)attachmentInfo.usageFlags
            };
            mode = formatInfo.mode;
        }

        ResizeRenderTexture(this, info.size);
    }

    VkRenderTexture::~VkRenderTexture()
    {
        ClearRenderTexture(this);
    }

    void BeginRendering(const VkRenderer* rn)
    {
        if (rn->currentRenderTexture->presentationAttachmentIndex != -1)
        {
            VkImageMemoryBarrier2 preBarrier =
            {
                .sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                .srcStageMask     = 0,
                .srcAccessMask    = 0,
                .dstStageMask     = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstAccessMask    = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                .oldLayout        = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = rn->swapchain.images[rn->imageIndex].image,
                .subresourceRange =
                {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                }
            };

            VkDependencyInfo depInfo
            {
                .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                .imageMemoryBarrierCount = 1,
                .pImageMemoryBarriers = &preBarrier,
            };

            GRAPHICS_DATA.extensionFunctions.vkCmdPipelineBarrier2KHR(rn->currentCommand, &depInfo);
        }

        auto tex = rn->currentRenderTexture;
        auto offset = tex->offset;
        auto extent = tex->extent;

        auto currentView = rn->swapchain.images[rn->imageIndex].view;

        VkRenderingInfoKHR renderingInfo
        {
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
            .pNext = nullptr,
            .renderArea =
            {
                .offset = offset,
                .extent = extent
            },
            .layerCount = 1,
            .viewMask = 0,
            .colorAttachmentCount = tex->colorAttachmentCount,
            .pColorAttachments = tex->colorAttachments
        };

        if ((uint32_t)(tex->mode & DepthStencilMode::Depth) > 0) renderingInfo.pDepthAttachment = &tex->depthStencilAttachment;
        if ((uint32_t)(tex->mode & DepthStencilMode::Stencil) > 0) renderingInfo.pStencilAttachment = &tex->depthStencilAttachment;

        if (tex->presentationAttachmentIndex != -1)
        {
            if (tex->multisampleMode != MultisampleMode::MSAA_1X)
                tex->colorAttachments[tex->presentationAttachmentIndex].resolveImageView = currentView;
            else tex->colorAttachments[tex->presentationAttachmentIndex].imageView = currentView;
        }

        GRAPHICS_DATA.extensionFunctions.vkCmdBeginRenderingKHR(rn->currentCommand, &renderingInfo);
        UploadViewportAndScissor(rn);
    }

    void EndRendering(const VkRenderer* rn)
    {
        GRAPHICS_DATA.extensionFunctions.vkCmdEndRenderingKHR(rn->currentCommand);

        if (rn->prevRenderTexture->presentationAttachmentIndex != -1)
        {
            VkImageMemoryBarrier2 postBarrier =
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                .dstStageMask = 0,
                .dstAccessMask = 0,
                .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = rn->swapchain.images[rn->imageIndex].image,
                .subresourceRange =
                {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                }
            };

            VkDependencyInfo depInfo
            {
                .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                .imageMemoryBarrierCount = 1,
                .pImageMemoryBarriers = &postBarrier,
            };

            GRAPHICS_DATA.extensionFunctions.vkCmdPipelineBarrier2KHR(rn->currentCommand, &depInfo);
        }
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

    DepthStencilFormatInfo ChooseDepthStencilFormat(DepthStencilMode mode,
        VkImageTiling tiling, VkFormatFeatureFlags features)
    {
        switch ((int)mode)
        {
        case 1:

            for (auto i : inter::rendering::DEPTH_FORMATS)
            {
                auto format = (VkFormat)i;
                if (CheckFormatSupport(format, tiling, features))
                {
                    DepthStencilFormatInfo info
                    {
                        .format = format,
                        .mode = DepthStencilMode::Depth
                    };

                    if (format != VK_FORMAT_D32_SFLOAT && format != VK_FORMAT_D16_UNORM)
                        info.mode |= DepthStencilMode::Stencil;
                    return info;
                }
            }

            throw GENERIC_EXCEPT("[Hyperflow]", "No suitable depth format found");
        case 2:

            for (auto i : inter::rendering::STENCIL_FORMATS)
            {
                auto format = (VkFormat)i;
                if (CheckFormatSupport(format, tiling, features))
                {
                    DepthStencilFormatInfo info
                    {
                        .format = format,
                        .mode = DepthStencilMode::Stencil
                    };

                    if (format != VK_FORMAT_S8_UINT) info.mode |= DepthStencilMode::Depth;
                    return info;
                }
            }

            throw GENERIC_EXCEPT("[Hyperflow]", "No suitable stencil format found");
        case 3:

            for (auto i : inter::rendering::DEPTH_STENCIL_FORMATS)
            {
                auto format = (VkFormat)i;
                if (CheckFormatSupport(format, tiling, features))
                {
                    return
                    {
                        .format = format,
                        .mode = DepthStencilMode::Stencil | DepthStencilMode::Depth
                    };
                }
            }

            throw GENERIC_EXCEPT("[Hyperflow]", "No suitable depthStencil format found");
        default: throw GENERIC_EXCEPT("[Hyperflow]", "Unused renderPass depthStencil attachment!!!");
        }
    }

    bool CheckFormatSupport(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features)
    {
        auto& prop = GRAPHICS_DATA.device.formatProps[(uint32_t)format];
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

    bool IsDepthFormat(VkFormat format)
    {
        return format == VK_FORMAT_D32_SFLOAT || format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
            format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D16_UNORM_S8_UINT || format == VK_FORMAT_D16_UNORM;
    }

    bool IsStencilFormat(VkFormat format)
    {
        return format == VK_FORMAT_S8_UINT || format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
            format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D16_UNORM_S8_UINT;
    }

    void ResizeRenderTexture(VkRenderTexture* tex, ivec2 newSize)
    {
        if (newSize.x <= 0 || newSize.y <= 0 ||
            (newSize.x == tex->extent.width && newSize.y == tex->extent.height)) return;
        ClearRenderTexture(tex);

        for (uint32_t i = 0; i < tex->colorAttachmentCount; i++)
        {
            if (i == tex->presentationAttachmentIndex) continue;

            auto& attachment = tex->colorAttachments[i];
            auto& info = tex->colorInfos[i];

            inter::rendering::TextureCreationInfo_i textureInfo
            {
                .type = inter::rendering::TextureType::Tex2D,
                .viewType = inter::rendering::TextureViewType::Tex2D,
                .size = uvec3(newSize, 1),
                .channel = TextureChannel::RGBA,
                .mipLevels = 1,
                .samples = tex->multisampleMode,
                .pTextures = nullptr,
                .textureCount = 1,
                .details
                {
                    .format = (TextureFormat)info.format,
                    .aspectFlags = TextureAspectFlags::Color,
                    .tiling = TextureTiling::Optimal,
                    .usageFlags = TextureUsageFlags::Color,
                    .memoryType = BufferMemoryType::Static,
                    .finalLayout = (TextureResultLayoutType)attachment.imageLayout,
                }
            };

            if (tex->multisampleMode == MultisampleMode::MSAA_1X)
                textureInfo.details.usageFlags |= (TextureUsageFlags)info.usage;
            else textureInfo.details.usageFlags |= TextureUsageFlags::Transient;

            auto* texture = new VkTexture(textureInfo);
            tex->colorTextures.push_back(texture);

            attachment.imageLayout = (VkImageLayout)texture->details.finalLayout;

            if (tex->multisampleMode == MultisampleMode::MSAA_1X) attachment.imageView = texture->view;
            else attachment.resolveImageView = texture->view;
        }

        if (tex->mode != DepthStencilMode::None)
        {
            auto& attachment = tex->depthStencilAttachment;
            auto& info = tex->depthStencilInfo;

            inter::rendering::TextureCreationInfo_i textureInfo
            {
                .type = inter::rendering::TextureType::Tex2D,
                .viewType = inter::rendering::TextureViewType::Tex2D,
                .size = uvec3(newSize, 1),
                .channel = TextureChannel::RGBA,
                .mipLevels = 1,
                .samples = tex->multisampleMode,
                .pTextures = nullptr,
                .textureCount = 1,
                .details
                {
                    .format = (TextureFormat)info.format,
                    .tiling = TextureTiling::Optimal,
                    .usageFlags = TextureUsageFlags::DepthStencil | (TextureUsageFlags)info.usage,
                    .memoryType = BufferMemoryType::Static,
                    .finalLayout = (TextureResultLayoutType)attachment.imageLayout,
                }
            };

            textureInfo.details.aspectFlags = TextureAspectFlags::None;
            if ((uint32_t)(tex->mode & DepthStencilMode::Depth) > 0) textureInfo.details.aspectFlags |= TextureAspectFlags::Depth;
            if ((uint32_t)(tex->mode & DepthStencilMode::Stencil) > 0) textureInfo.details.aspectFlags |= TextureAspectFlags::Stencil;

            auto* texture = new VkTexture(textureInfo);
            tex->depthTexture = texture;
            attachment.imageView = texture->view;
        }

        if (tex->multisampleMode != MultisampleMode::MSAA_1X)
        {
            inter::rendering::TextureCreationInfo_i textureInfo
            {
                .type = inter::rendering::TextureType::Tex2D,
                .viewType = inter::rendering::TextureViewType::Tex2D,
                .size = uvec3(newSize, 1),
                .channel = TextureChannel::RGBA,
                .mipLevels = 1,
                .samples = tex->multisampleMode,
                .pTextures = nullptr,
                .textureCount = 1,
                .details
                {
                    .format = TextureFormat::Undefined,
                    .aspectFlags = TextureAspectFlags::Color,
                    .tiling = TextureTiling::Optimal,
                    .memoryType = BufferMemoryType::Static,
                    .finalLayout = TextureResultLayoutType::Color,
                }
            };

            tex->msaaTextures = std::vector<VkTexture*>(tex->colorAttachmentCount);
            for (uint32_t i = 0; i < tex->colorAttachmentCount; i++)
            {
                auto& info = tex->colorInfos[i];

                textureInfo.details.format = (TextureFormat)info.format;
                textureInfo.details.usageFlags = TextureUsageFlags::Color |
                                                 // TextureUsageFlags::Transient |
                                                 (TextureUsageFlags)info.usage;

                auto* texture = new VkTexture(textureInfo);
                tex->msaaTextures[i] = texture;

                auto& attachment = tex->colorAttachments[i];
                attachment.resolveImageLayout = (VkImageLayout)texture->details.finalLayout;
                attachment.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;

                if (i == tex->presentationAttachmentIndex) attachment.imageView = texture->view;
                else attachment.resolveImageView = texture->view;
            }
        }

        tex->extent = { (uint32_t)newSize.x, (uint32_t)newSize.y };
    }

    VkImage GetRenderTextureImage(const VkRenderTexture* tex, uint32_t imageIndex)
    {
        if (tex->colorTextures.size() > imageIndex) return tex->colorTextures[imageIndex]->image;
        return tex->depthTexture->image;
    }

    void ClearRenderTexture(VkRenderTexture* tex)
    {
        for (auto& t : tex->colorTextures) delete t;
        for (auto& t : tex->msaaTextures) delete t;
        tex->colorTextures.clear();
        tex->msaaTextures.clear();

        if (tex->depthTexture)
        {
            delete tex->depthTexture;
            tex->depthTexture = nullptr;
        }
    }
}