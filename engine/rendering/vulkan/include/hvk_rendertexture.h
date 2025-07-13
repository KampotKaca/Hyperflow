#ifndef HVK_RENDERTEXTURE_H
#define HVK_RENDERTEXTURE_H

#include "hvk_shared.h"
#include "hvk_texture.h"

namespace hf
{
    struct VkRenderTexture
    {
        explicit VkRenderTexture(const RenderTextureCreationInfo& info);
        ~VkRenderTexture();

        VkRenderingAttachmentInfoKHR colorAttachments[MAX_COLOR_ATTACHMENTS]{};
        VkRenderingAttachmentInfoKHR depthStencilAttachment{};

        VkFormat colorFormats[MAX_COLOR_ATTACHMENTS];
        VkFormat depthStencilFormat{};

        uint32_t colorAttachmentCount = 0;
        int32_t presentationAttachmentIndex = -1;
        DepthStencilMode mode = DepthStencilMode::None;

        std::vector<VkTexture*> colorTextures{};
        std::vector<VkTexture*> msaaTextures{};
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
