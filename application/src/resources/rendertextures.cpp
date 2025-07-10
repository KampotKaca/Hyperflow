#include "resources/rendertextures.h"
#include "../../appconfig.h"

namespace app
{
    AppRenderTextures APP_RENDER_TEXTURES{};

    void CreateRenderTextures()
    {
        {
            hf::RenderTextureDepthAttachmentInfo depthAttachment
            {
                .layout = hf::TextureResultLayoutType::DepthStencil,
                .lsOperation = hf::LoadStoreOperationType::ClearAndDontCare,
                .lsStencilOperation = hf::LoadStoreOperationType::DontCareAndDontCare,
            };

            hf::RenderTextureColorAttachmentInfo colorAttachment
            {
                .layout = hf::TextureResultLayoutType::Color,
                .format = hf::TextureFormat::R8G8B8A8_Unorm,
                .lsOperation = hf::LoadStoreOperationType::ClearAndDontCare,
                .lsStencilOperation = hf::LoadStoreOperationType::DontCareAndDontCare,
                .clearColor = { 0.0f, 0.0f, 0.0f, 1.0f },
            };

            const hf::RenderTextureCreationInfo info
            {
                .pColorAttachments = { colorAttachment },
                .colorAttachmentCount = 1,
                .depthAttachment = depthAttachment,
                .multisampleMode = MSAA_MODE,
                .size = hf::GetSize(hf::GetMainWindow()),
                .offset = { 0, 0 }
            };

            APP_RENDER_TEXTURES.mainDrawRenderTexture = hf::Create(info);
        }

        // {
        //     hf::RenderTextureColorAttachmentInfo colorAttachment
        //     {
        //         .layout = hf::TextureResultLayoutType::Color,
        //         .format = hf::TextureFormat::R8G8B8A8_Srgb,
        //         .lsOperation = hf::LoadStoreOperationType::ClearAndDontCare,
        //         .lsStencilOperation = hf::LoadStoreOperationType::DontCareAndDontCare,
        //         .clearColor = { 0.0f, 0.0f, 0.0f, 1.0f },
        //     };
        //
        //     const hf::RenderTextureCreationInfo info
        //     {
        //         .pColorAttachments = { colorAttachment },
        //         .colorAttachmentCount = 1,
        //         .multisampleMode = MSAA_MODE,
        //         .size = hf::GetSize(hf::GetMainWindow()),
        //         .offset = { 0, 0 }
        //     };
        //
        //     APP_RENDER_TEXTURES.presentationRenderTexture = hf::Create(info);
        // }
    }
}