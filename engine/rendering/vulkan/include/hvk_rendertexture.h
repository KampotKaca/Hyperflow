#ifndef HVK_RENDERTEXTURE_H
#define HVK_RENDERTEXTURE_H

#include "hvk_shared.h"
#include "hvk_texture.h"

namespace hf
{
    struct ImageInfo
    {
        VkFormat format{};
        VkImageUsageFlags usage{};
    };

    struct VkRenderTexture
    {
        explicit VkRenderTexture(const RenderTextureCreationInfo& info);
        ~VkRenderTexture();

        VkRenderingAttachmentInfoKHR colorAttachments[MAX_COLOR_ATTACHMENTS]{};
        VkRenderingAttachmentInfoKHR depthStencilAttachment{};

        ImageInfo colorInfos[MAX_COLOR_ATTACHMENTS];
        ImageInfo depthStencilInfo{};

        uint32_t colorAttachmentCount = 0;
        int32_t presentationAttachmentIndex = -1;
        DepthStencilMode mode = DepthStencilMode::None;

        SmallList<VkTexture*, RN_NUM_MAX_COLOR_ATTACHMENTS> colorTextures{};
        SmallList<VkTexture*, RN_NUM_MAX_COLOR_ATTACHMENTS> msaaTextures{};
        VkTexture* depthTexture{};

        VkExtent2D extent{};
        VkOffset2D offset{};
        MultisampleMode multisampleMode{};
    };

    void ResizeRenderTexture(VkRenderTexture* tex, ivec2 newSize);
    void ClearRenderTexture(VkRenderTexture* tex);
    VkImage GetRenderTextureImage(const VkRenderTexture* tex, uint32_t imageIndex);
}

#endif //HVK_RENDERTEXTURE_H
