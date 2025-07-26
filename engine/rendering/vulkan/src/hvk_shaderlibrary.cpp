#include "hvk_shaderlibrary.h"
#include "hvk_graphics.h"

namespace hf
{
    static VkShaderModule AddShaderStage(const void* code, uint32_t codeSize, VkShaderStageFlagBits stage, VkPipelineShaderStageCreateInfo* result);
    static void GetMultisampling(MultisampleMode mode, VkPipelineMultisampleStateCreateInfo& result);

    VkShaderLibrary::VkShaderLibrary(const inter::rendering::ShaderLibraryCreationInfo_i& info)
    {
        uint32_t moduleCount = info.vertexInputModuleCount + info.preRasterModuleCount + info.fragmentModuleCount + info.fragmentOutputModuleCount;
        auto shaderModules = std::vector<VkShaderModule>();
        auto stageCreateInfos = std::vector<VkPipelineShaderStageCreateInfo>(info.preRasterModuleCount * 4 + info.fragmentModuleCount);

        VkGraphicsPipelineCreateInfo pipelineCreateInfos[moduleCount];
        VkPipelineVertexInputStateCreateInfo vertexInputInfos[info.vertexInputModuleCount];
        VkPipelineRasterizationStateCreateInfo rasterizers[info.preRasterModuleCount];
        VkPipelineDepthStencilStateCreateInfo depthStencilStates[info.fragmentModuleCount];

        VkPipelineRenderingCreateInfo renderingInfos[info.fragmentOutputModuleCount];
        VkPipelineColorBlendAttachmentState colorBlendAttachments[info.fragmentOutputModuleCount];
        VkPipelineColorBlendStateCreateInfo colorBlendStateInfos[info.fragmentOutputModuleCount];

        modules = std::vector<VkPipeline>(moduleCount);

        uint32_t moduleIndex = 0;
        uint32_t stageInfoIndex = 0;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        GetMultisampling(info.sampleMode, multisampling);

        for (uint32_t i = 0; i < info.vertexInputModuleCount; i++)
        {
            auto& moduleInfo = info.pVertexInputModules[i];
            static constexpr VkGraphicsPipelineLibraryCreateInfoEXT vertexInputLibInfo
            {
                .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT,
                .flags = VK_GRAPHICS_PIPELINE_LIBRARY_VERTEX_INPUT_INTERFACE_BIT_EXT
            };

            static constexpr  VkPipelineInputAssemblyStateCreateInfo inputAssembly
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                .primitiveRestartEnable = VK_FALSE
            };

            const auto& attribute = GetAttrib(moduleInfo.attribute);
            vertexInputInfos[i] =
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                .vertexBindingDescriptionCount = 1,
                .pVertexBindingDescriptions = &attribute->bindingDescription,
                .vertexAttributeDescriptionCount = (uint32_t)attribute->attribDescriptions.size(),
                .pVertexAttributeDescriptions = attribute->attribDescriptions.data(),
            };

            VkGraphicsPipelineCreateInfo vertexInputPipelineInfo
            {
                .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                .pNext = &vertexInputLibInfo,
                .flags = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR,
                .pVertexInputState = &vertexInputInfos[i],
                .pInputAssemblyState = &inputAssembly,
            };

            pipelineCreateInfos[moduleIndex] = vertexInputPipelineInfo;
            moduleIndex++;
        }

#define ADD_MODULE(x)\
        {\
            auto module = x;\
            if (module)\
            {\
                stageCount++;\
                shaderModules.push_back(module);\
            }\
        }

        for (uint32_t i = 0; i < info.preRasterModuleCount; i++)
        {
            const auto& moduleInfo = info.pPreRasterModules[i];
            static constexpr VkGraphicsPipelineLibraryCreateInfoEXT preRasterLibInfo
            {
                .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT,
                .flags = VK_GRAPHICS_PIPELINE_LIBRARY_PRE_RASTERIZATION_SHADERS_BIT_EXT
            };

            static constexpr VkViewport dummyViewport
            {
                .x = 0.0f,
                .y = 0.0f,
                .width = 1.0f,
                .height = 1.0f,
                .minDepth = 0.0f,
                .maxDepth = 1.0f
            };

            static constexpr VkRect2D dummyScissor
            {
                .offset = { 0, 0 },
                .extent = { 1, 1 }
            };

            static constexpr VkPipelineViewportStateCreateInfo viewportState
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                .viewportCount = 1,
                .pViewports = &dummyViewport,
                .scissorCount = 1,
                .pScissors = &dummyScissor,
            };

            uint32_t stageCount = 0;
            ADD_MODULE(AddShaderStage(moduleInfo.vertexShaderCode, moduleInfo.vertexShaderCodeSize, VK_SHADER_STAGE_VERTEX_BIT, &stageCreateInfos[stageInfoIndex + stageCount]))
            ADD_MODULE(AddShaderStage(moduleInfo.tessellationControlShaderCode, moduleInfo.tessellationControlShaderCodeSize, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, &stageCreateInfos[stageInfoIndex + stageCount]));
            ADD_MODULE(AddShaderStage(moduleInfo.tessellationEvaluationShaderCode, moduleInfo.tessellationEvaluationShaderCodeSize, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, &stageCreateInfos[stageInfoIndex + stageCount]));
            ADD_MODULE(AddShaderStage(moduleInfo.geometryShaderCode, moduleInfo.geometryShaderCodeSize, VK_SHADER_STAGE_GEOMETRY_BIT, &stageCreateInfos[stageInfoIndex + stageCount]));

            rasterizers[i] =
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                .depthClampEnable = VK_FALSE,
                .rasterizerDiscardEnable = VK_FALSE,
                .polygonMode = VK_POLYGON_MODE_FILL,
                .cullMode = (VkCullModeFlags)moduleInfo.options.cullMode,
                .frontFace = (VkFrontFace)moduleInfo.options.faceDirection,
                .depthBiasEnable = VK_FALSE,
                .depthBiasConstantFactor = 0.0f,
                .depthBiasClamp = 0.0f,
                .depthBiasSlopeFactor = 0.0f,
                .lineWidth = 1.0f,
            };

            VkGraphicsPipelineCreateInfo preRasterPipelineInfo
            {
                .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                .pNext = &preRasterLibInfo,
                .flags = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR,
                .stageCount = stageCount,
                .pStages = &stageCreateInfos[stageInfoIndex],
                .pViewportState = &viewportState,
                .pRasterizationState = &rasterizers[i],
                .layout = GetShaderLayout(moduleInfo.layout)->layout,
            };

            pipelineCreateInfos[moduleIndex] = preRasterPipelineInfo;
            moduleIndex++;
            stageInfoIndex += 4;
        }

        for (uint32_t i = 0; i < info.fragmentModuleCount; i++)
        {
            const auto& moduleInfo = info.pFragmentModules[i];
            static constexpr VkGraphicsPipelineLibraryCreateInfoEXT fragmentLibInfo
            {
                .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT,
                .flags = VK_GRAPHICS_PIPELINE_LIBRARY_FRAGMENT_SHADER_BIT_EXT
            };

            uint32_t stageCount = 0;
            ADD_MODULE(AddShaderStage(moduleInfo.fragmentShaderCode, moduleInfo.fragmentShaderCodeSize, VK_SHADER_STAGE_FRAGMENT_BIT, &stageCreateInfos[stageInfoIndex + stageCount]));

            depthStencilStates[i] =
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
                .depthTestEnable = moduleInfo.depthStencilOptions.enableDepth,
                .depthWriteEnable = moduleInfo.depthStencilOptions.writeDepth,
                .depthCompareOp = (VkCompareOp)moduleInfo.depthStencilOptions.comparisonFunc,
                .depthBoundsTestEnable = moduleInfo.depthStencilOptions.enableDepthBounds,
                .stencilTestEnable = moduleInfo.depthStencilOptions.enableStencil,
                .front = (VkStencilOpState)(VkStencilOp)moduleInfo.depthStencilOptions.frontStencil,
                .back = (VkStencilOpState)(VkStencilOp)moduleInfo.depthStencilOptions.backStencil,
                .minDepthBounds = moduleInfo.depthStencilOptions.depthBounds.x,
                .maxDepthBounds = moduleInfo.depthStencilOptions.depthBounds.y
            };

            VkGraphicsPipelineCreateInfo fragmentPipelineInfo
            {
                .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                .pNext = &fragmentLibInfo,
                .flags = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR,
                .stageCount = stageCount,
                .pStages = &stageCreateInfos[stageInfoIndex],
                .pMultisampleState = &multisampling,
                .pDepthStencilState = &depthStencilStates[i],
                .layout = GetShaderLayout(moduleInfo.layout)->layout,
            };

            pipelineCreateInfos[moduleIndex] = fragmentPipelineInfo;
            moduleIndex++;
            stageInfoIndex++;
        }

#undef ADD_MODULE

        for (uint32_t i = 0; i < info.fragmentOutputModuleCount; i++)
        {
            const auto& moduleInfo = info.pFragmentOutputModules[i];

            static constexpr VkGraphicsPipelineLibraryCreateInfoEXT fragmentOutputLibInfo
            {
                .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT,
                .flags = VK_GRAPHICS_PIPELINE_LIBRARY_FRAGMENT_OUTPUT_INTERFACE_BIT_EXT
            };

            renderingInfos[i] =
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
                .pNext = &fragmentOutputLibInfo,
                .colorAttachmentCount = moduleInfo.drawOutputFormats.colorFormatCount,
                .pColorAttachmentFormats = (VkFormat*)moduleInfo.drawOutputFormats.colorFormats,
                .depthAttachmentFormat = (VkFormat)moduleInfo.drawOutputFormats.depthFormat,
                .stencilAttachmentFormat = (VkFormat)moduleInfo.drawOutputFormats.stencilFormat
            };

            outputFormats[moduleIndex] = moduleInfo.drawOutputFormats;

            VkPipelineColorBlendAttachmentState colorBlendAttachment
            {
                .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
            };

            if (moduleInfo.blendingOptions.blendMode == ShaderBlendMode::Alpha)
            {
                colorBlendAttachment.blendEnable = VK_TRUE;
                colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
                colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
                colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
            }
            else if (moduleInfo.blendingOptions.blendMode == ShaderBlendMode::None)
            {
                colorBlendAttachment.blendEnable = VK_FALSE;
                colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
                colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
                colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
                colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
                colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
            }

            colorBlendAttachments[i] = colorBlendAttachment;

            colorBlendStateInfos[i] =
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                .logicOpEnable = moduleInfo.blendingOptions.blendMode == ShaderBlendMode::Logical,
                .logicOp = (VkLogicOp)moduleInfo.blendingOptions.blendOp,
                .attachmentCount = 1,
                .pAttachments = &colorBlendAttachments[i],
                .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f },
            };

            VkGraphicsPipelineCreateInfo fragmentOutputPipelineInfo
            {
                .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                .pNext = &renderingInfos[i],
                .flags = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR,
                .pMultisampleState = &multisampling,
                .pColorBlendState = &colorBlendStateInfos[i],
            };

            pipelineCreateInfos[moduleIndex] = fragmentOutputPipelineInfo;
            moduleIndex++;
        }

        VK_HANDLE_EXCEPT(vkCreateGraphicsPipelines(GRAPHICS_DATA.device.logicalDevice.device,
            VK_NULL_HANDLE, moduleCount, pipelineCreateInfos, nullptr, modules.data()));

        for (uint32_t i = 0; i < shaderModules.size(); i++)
            vkDestroyShaderModule(GRAPHICS_DATA.device.logicalDevice.device, shaderModules[i], nullptr);
    }

    VkShaderLibrary::~VkShaderLibrary()
    {
        for (uint32_t i = 0; i < modules.size(); i++)
            vkDestroyPipeline(GRAPHICS_DATA.device.logicalDevice.device, modules[i], nullptr);
        modules.clear();
    }

    VkShaderModule AddShaderStage(const void* code, uint32_t codeSize, VkShaderStageFlagBits stage, VkPipelineShaderStageCreateInfo* result)
    {
        if (codeSize > 0 && code != nullptr)
        {
            VkShaderModule module;

            const VkShaderModuleCreateInfo createInfo
            {
                .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                .codeSize = codeSize,
                .pCode = (uint32_t*)code,
            };
            VK_HANDLE_EXCEPT(vkCreateShaderModule(GRAPHICS_DATA.device.logicalDevice.device, &createInfo, nullptr, &module));

            *result =
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = stage,
                .module = module,
                .pName = "main"
            };
            return module;
        }

        return nullptr;
    }

    void GetMultisampling(MultisampleMode mode, VkPipelineMultisampleStateCreateInfo& result)
    {
        VkPipelineMultisampleStateCreateInfo multisampling
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = (VkSampleCountFlagBits)mode,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE
        };

        if (mode != MultisampleMode::MSAA_1X &&
            GRAPHICS_DATA.device.features.sampleRateShading)
        {
            multisampling.sampleShadingEnable = VK_TRUE;
            multisampling.minSampleShading = VK_MSAA_MIN_SAMPLE_SHADING;
        }
        else
        {
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.minSampleShading = 1.0f;
        }

        result = std::move(multisampling);
    }

    VkPipeline GetShaderLibraryModule(const VkShaderLibrary* lib, uint32_t moduleId)
    {
        if (lib->modules.size() > moduleId) return lib->modules[moduleId];
        throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access out of bounds shader module! ID: %i", moduleId);
    }
}