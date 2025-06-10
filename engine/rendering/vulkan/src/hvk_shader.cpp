#include "hvk_shader.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    struct VkPipelineInfo
    {
        uint32_t stageCount = 0;
        VkPipelineShaderStageCreateInfo* pStages = nullptr;
        BufferAttrib attrib{};
        VkPipelineLayout layout{};
        RenderPass renderPass{};
        ShaderBlendingOptions blendingOptions{};
        ShaderDepthStencilOptions depthStencilOptions{};
    };

    static void CreateShaderModule(const char* code, uint32_t codeSize, VkShaderModule* result);
    static void CreatePipeline(const VkPipelineInfo& info, VkPipeline* pipeline);

    VkShader::VkShader(const inter::rendering::ShaderCreationInfo& info)
        : shaderSetup(info.shaderSetup)
    {
        VkShaderModule vertModule{}, fragModule{};
        CreateShaderModule(info.vCode, info.vCodeSize, &vertModule);
        CreateShaderModule(info.fCode, info.fCodeSize, &fragModule);

        VkPipelineShaderStageCreateInfo shaderStages[] =
        {
            {//vertex
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = vertModule,
                .pName = "main",
            },
            {//fragment
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = fragModule,
                .pName = "main"
            }
        };

        auto& shaderSetup = GetShaderSetup(info.shaderSetup);
        VkPipelineInfo pipelineCreationInfo
        {
            .stageCount = 2,
            .pStages = shaderStages,
            .layout = shaderSetup->layout,
            .renderPass = info.renderPass,
            .blendingOptions = info.blendingOptions,
            .depthStencilOptions = info.depthStencilOptions,
        };

        for (uint32_t i = 0; i < info.supportedAttribCount; i++)
        {
            auto attrib = info.pSupportedAttribs[i];
            pipelineCreationInfo.attrib = attrib;
            VkPipeline pipeline{};
            CreatePipeline(pipelineCreationInfo, &pipeline);
            pipelines[attrib] = pipeline;
        }

        auto& device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        vkDestroyShaderModule(device, vertModule, nullptr);
        vkDestroyShaderModule(device, fragModule, nullptr);
    }

    VkShader::~VkShader()
    {
        auto& device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        for (auto& pipeline : pipelines | std::views::values)
            vkDestroyPipeline(device, pipeline, nullptr);

        pipelines.clear();
    }

    void CreateShaderModule(const char* code, uint32_t codeSize, VkShaderModule* result)
    {
        using namespace inter;
        VkShaderModuleCreateInfo createInfo
        {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = codeSize,
            .pCode = (uint32_t*)code,
        };
        VK_HANDLE_EXCEPT(vkCreateShaderModule(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
            &createInfo, nullptr, result));
    }

    void CreatePipeline(const VkPipelineInfo& info, VkPipeline* pipeline)
    {
        const auto& attribute = GetAttrib(info.attrib);
        auto& pass = GetRenderPass(info.renderPass);

        VkPipelineVertexInputStateCreateInfo vertexInputInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = 1,
            .pVertexBindingDescriptions = &attribute->bindingDescription,
            .vertexAttributeDescriptionCount = (uint32_t)attribute->attribDescriptions.size(),
            .pVertexAttributeDescriptions = attribute->attribDescriptions.data(),
        };

        VkPipelineInputAssemblyStateCreateInfo inputAssembly
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE
        };

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

        VkPipelineViewportStateCreateInfo viewportState
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .pViewports = nullptr,
            .scissorCount = 1,
            .pScissors = nullptr,
        };

        VkPipelineRasterizationStateCreateInfo rasterizer
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
            .lineWidth = 1.0f,
        };

        VkPipelineMultisampleStateCreateInfo multisampling
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE,
        };

        if (pass->msaaSamples.size() > 0)
        {
            uint32_t maxSamples = pass->msaaSamples[0];
            for (uint32_t i = 1; i < pass->msaaSamples.size(); i++)
                if (maxSamples < pass->msaaSamples[i]) maxSamples = pass->msaaSamples[i];
            multisampling.rasterizationSamples = (VkSampleCountFlagBits)maxSamples;
        }
        else multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        if (!pass->msaaSamples.empty() && GRAPHICS_DATA.defaultDevice->features.sampleRateShading)
        {
            multisampling.sampleShadingEnable = VK_TRUE;
            multisampling.minSampleShading = VK_MSAA_MIN_SAMPLE_SHADING;
        }
        else
        {
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.minSampleShading = 1.0f;
        }

        VkPipelineColorBlendAttachmentState colorBlendAttachment
        {
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        };

        if (info.blendingOptions.blendMode == ShaderBlendMode::Alpha)
        {
            colorBlendAttachment.blendEnable = VK_TRUE;
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        }
        else if (info.blendingOptions.blendMode == ShaderBlendMode::None)
        {
            colorBlendAttachment.blendEnable = VK_FALSE;
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        }

        VkPipelineColorBlendStateCreateInfo colorBlending
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = info.blendingOptions.blendMode == ShaderBlendMode::Logical,
            .logicOp = (VkLogicOp)info.blendingOptions.blendOp,
            .attachmentCount = 1,
            .pAttachments = &colorBlendAttachment,
            .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f },
        };

        VkGraphicsPipelineCreateInfo pipelineInfo
        {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = info.stageCount,
            .pStages = info.pStages,
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssembly,
            .pViewportState = &viewportState,
            .pRasterizationState = &rasterizer,
            .pMultisampleState = &multisampling,
            .pColorBlendState = &colorBlending,
            .pDynamicState = &dynamicState,
            .layout = info.layout,
            .renderPass = pass->pass,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1,
        };

        auto& dsOptions = info.depthStencilOptions;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = dsOptions.enableDepth,
            .depthWriteEnable = dsOptions.writeDepth,
            .depthCompareOp = (VkCompareOp)dsOptions.comparisonFunc,
            .depthBoundsTestEnable = dsOptions.enableDepthBounds,
            .stencilTestEnable = dsOptions.enableStencil,
            .front = (VkStencilOpState)(VkStencilOp)dsOptions.frontStencil,
            .back = (VkStencilOpState)(VkStencilOp)dsOptions.backStencil,
            .minDepthBounds = dsOptions.depthBounds.x,
            .maxDepthBounds = dsOptions.depthBounds.y
        };

        if (pass->depthAttachmentRefs.size() > 0)
            pipelineInfo.pDepthStencilState = &depthStencilInfo;
        else pipelineInfo.pDepthStencilState = nullptr;

        VK_HANDLE_EXCEPT(vkCreateGraphicsPipelines(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
            VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, pipeline));
    }

    void BindShader(VkRenderer* rn, VkShader* shader, BufferAttrib attrib, RenderBindingType bindingType)
    {
#if DEBUG
        if (rn->currentLayout != GetShaderSetup(shader->shaderSetup)->layout)
            throw GENERIC_EXCEPT("[Hyperflow]", "Bind correct shader setup first");
#endif

        vkCmdBindPipeline(rn->currentCommand, (VkPipelineBindPoint)bindingType, shader->pipelines[attrib]);
    }
}