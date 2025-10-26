#ifndef HVK_SHADERLIBRARY_H
#define HVK_SHADERLIBRARY_H

#include "hvk_shared.h"

namespace hf
{
    struct VkShaderLibrary
    {
        explicit VkShaderLibrary(const inter::rendering::ShaderLibraryCreationInfo_i& info);
        ~VkShaderLibrary();

        ShaderDrawOutputFormats outputFormats{};
        List<VkPipeline> modules{};
    };

    VkPipeline GetShaderLibraryModule(const VkShaderLibrary* lib, uint32_t moduleId);
    inline void GetRenderingInfo(const ShaderDrawOutputFormats& outputFormat, VkPipelineRenderingCreateInfo& result)
    {
        VkPipelineRenderingCreateInfo renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
        renderingInfo.colorAttachmentCount = outputFormat.colorFormatCount;
        renderingInfo.pColorAttachmentFormats = (VkFormat*)outputFormat.colorFormats;
        renderingInfo.depthAttachmentFormat = (VkFormat)outputFormat.depthFormat;
        renderingInfo.stencilAttachmentFormat = (VkFormat)outputFormat.stencilFormat;
        result = renderingInfo;
    }
}

#endif //HVK_SHADERLIBRARY_H
