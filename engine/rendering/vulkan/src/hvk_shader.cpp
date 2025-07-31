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

        VkPipelineRenderingCreateInfo renderingInfo{};
        GetRenderingInfo(library->outputFormats, renderingInfo);

        const std::array libs
        {
            GetShaderLibraryModule(library, info.modules.vertexInputModuleId),
            GetShaderLibraryModule(library, info.modules.preRasterModuleId),
            GetShaderLibraryModule(library, info.modules.fragmentModuleId),
            GetShaderLibraryModule(library, info.modules.fragmentOutputModuleId),
        };

        VkPipelineLibraryCreateInfoKHR libraryInfo{};
        libraryInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LIBRARY_CREATE_INFO_KHR;
        libraryInfo.pNext = &renderingInfo;
        libraryInfo.libraryCount = libs.size();
        libraryInfo.pLibraries = libs.data();

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext = &libraryInfo;
        pipelineInfo.flags = VK_PIPELINE_CREATE_DESCRIPTOR_BUFFER_BIT_EXT;
        pipelineInfo.pDynamicState = &SHADER_DYNAMIC;
        pipelineInfo.layout = shaderLayout->layout;

        VK_HANDLE_EXCEPT(vkCreateGraphicsPipelines(GRAPHICS_DATA.device.logicalDevice.device,
            VK_NULL_HANDLE, 1, &pipelineInfo, &GRAPHICS_DATA.platform.allocator, &pipeline));
    }

    VkShader::~VkShader()
    {
        if (pipeline)
        {
            vkDestroyPipeline(GRAPHICS_DATA.device.logicalDevice.device, pipeline, &GRAPHICS_DATA.platform.allocator);
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