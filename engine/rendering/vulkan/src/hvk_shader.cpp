#include "hvk_shader.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"
#include "hvk_shaderlibrary.h"

namespace hf
{
    VkShader::VkShader(const inter::rendering::ShaderCreationInfo_i& info)
        : layout(info.layout)
    {
        const auto& shaderLayout = GetShaderLayout(info.layout);
        const auto library = (VkShaderLibrary*)info.library;
        auto& outputFormat = library->outputFormats[info.modules.fragmentOutputModuleId];

        std::vector dynamicStates =
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicState
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = (uint32_t)dynamicStates.size(),
            .pDynamicStates = dynamicStates.data()
        };

        std::array libs
        {
            GetShaderLibraryModule(library, info.modules.vertexInputModuleId),
            GetShaderLibraryModule(library, info.modules.preRasterModuleId),
            GetShaderLibraryModule(library, info.modules.fragmentModuleId),
            GetShaderLibraryModule(library, info.modules.fragmentOutputModuleId),
        };

        VkPipelineLibraryCreateInfoKHR libraryInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LIBRARY_CREATE_INFO_KHR,
            .libraryCount = libs.size(),
            .pLibraries = libs.data()
        };

        VkPipelineRenderingCreateInfo renderingInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
            .pNext = &libraryInfo,
            .colorAttachmentCount = outputFormat.colorFormatCount,
            .pColorAttachmentFormats = (VkFormat*)outputFormat.colorFormats,
            .depthAttachmentFormat = (VkFormat)outputFormat.depthFormat,
            .stencilAttachmentFormat = (VkFormat)outputFormat.stencilFormat
        };

        const VkGraphicsPipelineCreateInfo pipelineInfo
        {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = &renderingInfo,
            .pDynamicState = &dynamicState,
            .layout = shaderLayout->layout,
        };

        VK_HANDLE_EXCEPT(vkCreateGraphicsPipelines(GRAPHICS_DATA.device.logicalDevice.device,
            VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline));
    }

    VkShader::~VkShader()
    {
        if (pipeline)
        {
            vkDestroyPipeline(GRAPHICS_DATA.device.logicalDevice.device, pipeline, nullptr);
            pipeline = VK_NULL_HANDLE;
        }
    }

    void BindShader(const VkRenderer* rn, const VkShader* shader)
    {
#if DEBUG
        if (rn->currentLayout != GetShaderLayout(shader->layout)->layout)
            throw GENERIC_EXCEPT("[Hyperflow]", "Bind correct shader setup first");
#endif

        vkCmdBindPipeline(rn->currentCommand, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->pipeline);
    }
}