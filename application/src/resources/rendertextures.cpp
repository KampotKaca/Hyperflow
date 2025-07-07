#include "resources/rendertextures.h"
#include "../../appconfig.h"

namespace app
{
    AppRenderTextures APP_RENDER_TEXTURES{};

    void CreateRenderTextures()
    {
        hf::RenderSubpassPresentationAttachmentInfo presentAttachment{};
        hf::RenderSubpassDepthAttachmentInfo depthAttachment
        {
            .layout = hf::TextureResultLayoutType::DepthStencil,
            .lsOperation = hf::LoadStoreOperationType::ClearAndDontCare,
            .lsStencilOperation = hf::LoadStoreOperationType::DontCareAndDontCare,
        };

        const hf::RenderTextureCreationInfo info
        {
            .presentationAttachment = std::move(presentAttachment),
            .depthAttachment = depthAttachment,
            .multisampleMode = MSAA_MODE,
            .size = hf::GetSize(hf::GetMainWindow()),
            .offset = { 0, 0 }
        };

        APP_RENDER_TEXTURES.mainRenderTexture = hf::Create(info);
        Attach(GetRenderer(hf::GetMainWindow()), APP_RENDER_TEXTURES.mainRenderTexture);
    }
}