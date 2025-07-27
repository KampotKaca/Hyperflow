#include "hvk_shaderlibrary.h"
#include "hvk_graphics.h"

namespace hf
{
    static VkShaderModule AddShaderStage(const void* code, uint32_t codeSize, VkShaderStageFlagBits stage, VkPipelineShaderStageCreateInfo* result);

    VkShaderLibrary::VkShaderLibrary(const inter::rendering::ShaderLibraryCreationInfo_i& info) : outputFormats(info.outputFormats)
    {
        uint32_t moduleCount = info.vertexInputModuleCount + info.preRasterModuleCount + info.fragmentModuleCount + info.fragmentOutputModuleCount;
        uint32_t maxShadingModules = info.preRasterModuleCount * 4 + info.fragmentModuleCount;

        std::vector<VkShaderModule> shaderModules(maxShadingModules);
        std::vector<VkPipelineShaderStageCreateInfo> stageCreateInfos(maxShadingModules);

        std::vector<VkGraphicsPipelineCreateInfo> pipelineCreateInfos(moduleCount);
        std::vector<VkPipelineVertexInputStateCreateInfo> vertexInputInfos(info.vertexInputModuleCount);
        std::vector<VkPipelineRasterizationStateCreateInfo> rasterizers(info.preRasterModuleCount);
        std::vector<VkPipelineDepthStencilStateCreateInfo> depthStencilStates(info.fragmentModuleCount);

        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments(info.fragmentOutputModuleCount);
        std::vector<VkPipelineColorBlendStateCreateInfo> colorBlendStateInfos(info.fragmentOutputModuleCount);

        modules = std::vector<VkPipeline>(moduleCount);

        uint32_t moduleIndex = 0;
        uint32_t stageInfoCount = 0;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.rasterizationSamples = (VkSampleCountFlagBits)outputFormats.sampleMode;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;

        if (outputFormats.sampleMode != MultisampleMode::MSAA_1X &&
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

        VkPipelineRenderingCreateInfo renderingInfo{};
        GetRenderingInfo(outputFormats, renderingInfo);

        VkGraphicsPipelineLibraryCreateInfoEXT vertexInputLibInfo{};
        vertexInputLibInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT;
        vertexInputLibInfo.pNext = &renderingInfo;
        vertexInputLibInfo.flags = VK_GRAPHICS_PIPELINE_LIBRARY_VERTEX_INPUT_INTERFACE_BIT_EXT;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkGraphicsPipelineLibraryCreateInfoEXT preRasterLibInfo{};
        preRasterLibInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT;
        preRasterLibInfo.pNext = &renderingInfo;
        preRasterLibInfo.flags = VK_GRAPHICS_PIPELINE_LIBRARY_PRE_RASTERIZATION_SHADERS_BIT_EXT;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkGraphicsPipelineLibraryCreateInfoEXT fragmentLibInfo{};
        fragmentLibInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT;
        fragmentLibInfo.pNext = &renderingInfo;
        fragmentLibInfo.flags = VK_GRAPHICS_PIPELINE_LIBRARY_FRAGMENT_SHADER_BIT_EXT;

        VkGraphicsPipelineLibraryCreateInfoEXT fragmentOutputLibInfo{};
        fragmentOutputLibInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT;
        fragmentOutputLibInfo.pNext = &renderingInfo;
        fragmentOutputLibInfo.flags = VK_GRAPHICS_PIPELINE_LIBRARY_FRAGMENT_OUTPUT_INTERFACE_BIT_EXT;

        for (uint32_t i = 0; i < info.vertexInputModuleCount; i++)
        {
            auto& moduleInfo = info.pVertexInputModules[i];

            const auto& attribute = GetAttrib(moduleInfo.attribute);
            VkPipelineVertexInputStateCreateInfo vInfo{};
            vInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vInfo.vertexBindingDescriptionCount = 1;
            vInfo.pVertexBindingDescriptions = &attribute->bindingDescription;
            vInfo.vertexAttributeDescriptionCount = (uint32_t)attribute->attribDescriptions.size();
            vInfo.pVertexAttributeDescriptions = attribute->attribDescriptions.data();
            vertexInputInfos[i] = vInfo;

            VkGraphicsPipelineCreateInfo vertexInputPipelineInfo{};
            vertexInputPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            vertexInputPipelineInfo.pNext = &vertexInputLibInfo;
            vertexInputPipelineInfo.flags = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR;
            vertexInputPipelineInfo.pVertexInputState = &vertexInputInfos[i];
            vertexInputPipelineInfo.pInputAssemblyState = &inputAssembly;

            pipelineCreateInfos[moduleIndex] = vertexInputPipelineInfo;
            moduleIndex++;
        }

#define ADD_MODULE(x)\
        {\
            auto module = x;\
            if (module)\
            {\
                shaderModules[stageInfoCount + stageCount] = module;\
                stageCount++;\
            }\
        }

        for (uint32_t i = 0; i < info.preRasterModuleCount; i++)
        {
            const auto& moduleInfo = info.pPreRasterModules[i];

            uint32_t stageCount = 0;
            ADD_MODULE(AddShaderStage(moduleInfo.vertexShaderCode, moduleInfo.vertexShaderCodeSize, VK_SHADER_STAGE_VERTEX_BIT, &stageCreateInfos[stageInfoCount + stageCount]))
            ADD_MODULE(AddShaderStage(moduleInfo.tessellationControlShaderCode, moduleInfo.tessellationControlShaderCodeSize, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, &stageCreateInfos[stageInfoCount + stageCount]));
            ADD_MODULE(AddShaderStage(moduleInfo.tessellationEvaluationShaderCode, moduleInfo.tessellationEvaluationShaderCodeSize, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, &stageCreateInfos[stageInfoCount + stageCount]));
            ADD_MODULE(AddShaderStage(moduleInfo.geometryShaderCode, moduleInfo.geometryShaderCodeSize, VK_SHADER_STAGE_GEOMETRY_BIT, &stageCreateInfos[stageInfoCount + stageCount]));

            VkPipelineRasterizationStateCreateInfo rasterInfo{};
            rasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterInfo.depthClampEnable = VK_FALSE;
            rasterInfo.rasterizerDiscardEnable = VK_FALSE;
            rasterInfo.polygonMode = VK_POLYGON_MODE_FILL;
            rasterInfo.cullMode = (VkCullModeFlags)moduleInfo.options.cullMode;
            rasterInfo.frontFace = (VkFrontFace)moduleInfo.options.faceDirection;
            rasterInfo.depthBiasEnable = VK_FALSE;
            rasterInfo.depthBiasConstantFactor = 0.0f;
            rasterInfo.depthBiasClamp = 0.0f;
            rasterInfo.depthBiasSlopeFactor = 0.0f;
            rasterInfo.lineWidth = 1.0f;
            rasterizers[i] = rasterInfo;

            VkGraphicsPipelineCreateInfo preRasterPipelineInfo{};
            preRasterPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            preRasterPipelineInfo.pNext = &preRasterLibInfo;
            preRasterPipelineInfo.flags = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR;
            preRasterPipelineInfo.stageCount = stageCount;
            preRasterPipelineInfo.pStages = &stageCreateInfos[stageInfoCount];
            preRasterPipelineInfo.pViewportState = &viewportState;
            preRasterPipelineInfo.pRasterizationState = &rasterizers[i];
            preRasterPipelineInfo.pDynamicState = &SHADER_DYNAMIC;
            preRasterPipelineInfo.layout = GetShaderLayout(moduleInfo.layout)->layout;
            pipelineCreateInfos[moduleIndex] = preRasterPipelineInfo;

            moduleIndex++;
            stageInfoCount += stageCount;
        }

        for (uint32_t i = 0; i < info.fragmentModuleCount; i++)
        {
            const auto& moduleInfo = info.pFragmentModules[i];

            uint32_t stageCount = 0;
            ADD_MODULE(AddShaderStage(moduleInfo.fragmentShaderCode, moduleInfo.fragmentShaderCodeSize, VK_SHADER_STAGE_FRAGMENT_BIT, &stageCreateInfos[stageInfoCount + stageCount]));

            VkPipelineDepthStencilStateCreateInfo depthStencilState{};
            depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencilState.depthTestEnable = moduleInfo.depthStencilOptions.enableDepth;
            depthStencilState.depthWriteEnable = moduleInfo.depthStencilOptions.writeDepth;
            depthStencilState.depthCompareOp = (VkCompareOp)moduleInfo.depthStencilOptions.comparisonFunc;
            depthStencilState.depthBoundsTestEnable = moduleInfo.depthStencilOptions.enableDepthBounds;
            depthStencilState.stencilTestEnable = moduleInfo.depthStencilOptions.enableStencil;
            depthStencilState.front = (VkStencilOpState)(VkStencilOp)moduleInfo.depthStencilOptions.frontStencil;
            depthStencilState.back = (VkStencilOpState)(VkStencilOp)moduleInfo.depthStencilOptions.backStencil;
            depthStencilState.minDepthBounds = moduleInfo.depthStencilOptions.depthBounds.x;
            depthStencilState.maxDepthBounds = moduleInfo.depthStencilOptions.depthBounds.y;
            depthStencilStates[i] = depthStencilState;

            VkGraphicsPipelineCreateInfo fragmentPipelineInfo{};
            fragmentPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            fragmentPipelineInfo.pNext = &fragmentLibInfo;
            fragmentPipelineInfo.flags = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR;
            fragmentPipelineInfo.stageCount = stageCount;
            fragmentPipelineInfo.pStages = &stageCreateInfos[stageInfoCount];
            fragmentPipelineInfo.pMultisampleState = &multisampling;
            fragmentPipelineInfo.pDepthStencilState = &depthStencilStates[i];
            fragmentPipelineInfo.layout = GetShaderLayout(moduleInfo.layout)->layout;
            pipelineCreateInfos[moduleIndex] = fragmentPipelineInfo;

            moduleIndex++;
            stageInfoCount += stageCount;
        }

#undef ADD_MODULE

        for (uint32_t i = 0; i < info.fragmentOutputModuleCount; i++)
        {
            const auto& moduleInfo = info.pFragmentOutputModules[i];

            VkPipelineColorBlendAttachmentState colorBlendAttachment{};
            colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

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

            VkPipelineColorBlendStateCreateInfo blendAttachmentInfo{};
            blendAttachmentInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            blendAttachmentInfo.logicOpEnable = moduleInfo.blendingOptions.blendMode == ShaderBlendMode::Logical;
            blendAttachmentInfo.logicOp = (VkLogicOp)moduleInfo.blendingOptions.blendOp;
            blendAttachmentInfo.attachmentCount = 1;
            blendAttachmentInfo.pAttachments = &colorBlendAttachments[i];
            colorBlendStateInfos[i] = blendAttachmentInfo;

            VkGraphicsPipelineCreateInfo fragmentOutputPipelineInfo{};
            fragmentOutputPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            fragmentOutputPipelineInfo.pNext = &fragmentOutputLibInfo;
            fragmentOutputPipelineInfo.flags = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR;
            fragmentOutputPipelineInfo.pMultisampleState = &multisampling;
            fragmentOutputPipelineInfo.pColorBlendState = &colorBlendStateInfos[i];

            pipelineCreateInfos[moduleIndex] = fragmentOutputPipelineInfo;
            moduleIndex++;
        }

        VK_HANDLE_EXCEPT(vkCreateGraphicsPipelines(GRAPHICS_DATA.device.logicalDevice.device,
            VK_NULL_HANDLE, (uint32_t)pipelineCreateInfos.size(), pipelineCreateInfos.data(), nullptr, modules.data()));

        for (uint32_t i = 0; i < stageInfoCount; i++)
            vkDestroyShaderModule(GRAPHICS_DATA.device.logicalDevice.device, shaderModules[i], nullptr);
    }

    VkShaderLibrary::~VkShaderLibrary()
    {
        for (const auto& module : modules)
            vkDestroyPipeline(GRAPHICS_DATA.device.logicalDevice.device, module, nullptr);
        modules.clear();
    }

    VkShaderModule AddShaderStage(const void* code, uint32_t codeSize, VkShaderStageFlagBits stage, VkPipelineShaderStageCreateInfo* result)
    {
        if (codeSize > 0 && code != nullptr)
        {
            VkShaderModule module{};
            VkShaderModuleCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = codeSize;
            createInfo.pCode = (uint32_t*)code;
            VK_HANDLE_EXCEPT(vkCreateShaderModule(GRAPHICS_DATA.device.logicalDevice.device, &createInfo, nullptr, &module));

            VkPipelineShaderStageCreateInfo stageInfo{};
            stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stageInfo.stage = stage;
            stageInfo.module = module;
            stageInfo.pName = "main";

            *result = stageInfo;
            return module;
        }

        return nullptr;
    }

    VkPipeline GetShaderLibraryModule(const VkShaderLibrary* lib, uint32_t moduleId)
    {
        if (lib->modules.size() > moduleId) return lib->modules[moduleId];
        throw GENERIC_EXCEPT("[Hyperflow]", "Trying to access out of bounds shader module! ID: %i", moduleId);
    }
}