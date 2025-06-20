#include "resources/renderpasses.h"

namespace app
{
    AppRenderPasses APP_RENDER_PASSES{};

    hf::RenderPass DefineMainPresentationPass(const hf::Ref<hf::Renderer>& rn)
    {
        hf::RenderPassDependencyInfo dependencyInfo
        {
            .src =
            {
                .stageMask = hf::RenderPassStage::ColorAttachmentOutput |
                             hf::RenderPassStage::EarlyFragmentTest,
            },
            .dst =
            {
                .stageMask = hf::RenderPassStage::ColorAttachmentOutput |
                             hf::RenderPassStage::EarlyFragmentTest,
                .accessMask = hf::AccessType::ColorAttachmentWrite |
                              hf::AccessType::DepthStencilAttachmentWrite
            },
            .flags = hf::RenderPassDependencyType::ByRegion
        };
        hf::RenderSubpassPresentationAttachmentInfo presentAttachment{};
        hf::RenderSubpassDepthAttachmentInfo depthAttachment
        {
            .layout = hf::TextureResultLayoutType::DepthStencil,
            .lsOperation = hf::LoadStoreOperationType::ClearAndDontCare,
            .lsStencilOperation = hf::LoadStoreOperationType::DontCareAndDontCare,
            .initialLayout = hf::TextureResultLayoutType::Undefined,
            .finalLayout = hf::TextureResultLayoutType::DepthStencil,
            .usesSharedMemory = false
        };

        hf::RenderSubpassInfo subpassInfo
        {
            .bindingType = hf::RenderBindingType::Graphics,
            .pColorAttachments = nullptr,
            .attachmentCount = 0,
            .msaaCounter = 2,
            .presentationAttachment = &presentAttachment,
            .depthAttachment = &depthAttachment
        };

        hf::RenderPassDefinitionInfo drawPassDefinitionInfo
        {
            .pSubpasses = &subpassInfo,
            .subpassCount = 1,
            .pDependencies = &dependencyInfo,
            .dependencyCount = 1,
        };
        APP_RENDER_PASSES.mainPresentPass = hf::DefineRenderPass(drawPassDefinitionInfo);
        rn->Bind(APP_RENDER_PASSES.mainPresentPass);

        return APP_RENDER_PASSES.mainPresentPass;
    }
}