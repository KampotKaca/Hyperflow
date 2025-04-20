#include "hvk_shader.h"
#include "hshader.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"
#include "exceptions/hgraphicsexception.h"

namespace hf::inter::rendering
{
    static void CreateShaderModule(const char* code, uint32_t codeSize, VkShaderModule* result);
    static void CreatePipelineLayout(const ShaderCreationInfo& info, VkPipelineLayout* pipelineLayout);
    static void CreatePipeline(const VkPipelineInfo& info, VkPipeline* pipeline);

    VkShader::VkShader(const ShaderCreationInfo& info, const Shader* shader)
    : shader(shader)
    {
        using namespace inter;
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

        CreatePipelineLayout(info, &pipelineLayout);

        VkPipelineInfo pipelineCreationInfo
        {
            .pStages = shaderStages,
            .stageCount = 2,
            .blendingMode = PipelineBlendType::None,
            .blendingOp = VK_LOGIC_OP_XOR,
            .layout = pipelineLayout,
            .renderPass = GRAPHICS_DATA.renderPass,
        };

        CreatePipeline(pipelineCreationInfo, &pipeline);

        auto& device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        vkDestroyShaderModule(device, vertModule, nullptr);
        vkDestroyShaderModule(device, fragModule, nullptr);
    }

    VkShader::~VkShader()
    {
        auto& device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        vkDestroyPipeline(device, pipeline, nullptr);
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    }

    void* CreateShader(const ShaderCreationInfo& info, const Shader* shader)
    {
        return new VkShader(info, shader);
    }

    void DestroyShader(void* shader)
    {
        delete (VkShader*)shader;
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

    void CreatePipelineLayout(const ShaderCreationInfo& info, VkPipelineLayout* pipelineLayout)
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 0,
            .pSetLayouts = nullptr,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr,
        };

        VK_HANDLE_EXCEPT(vkCreatePipelineLayout(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
            &pipelineLayoutInfo, nullptr, pipelineLayout));
    }

    void CreatePipeline(const VkPipelineInfo& info, VkPipeline* pipeline)
    {
        VkPipelineVertexInputStateCreateInfo vertexInputInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = 0,
            .pVertexBindingDescriptions = nullptr,
            .vertexAttributeDescriptionCount = 0,
            .pVertexAttributeDescriptions = nullptr
        };

        VkPipelineInputAssemblyStateCreateInfo inputAssembly
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE
        };

        std::vector<VkDynamicState> dynamicStates =
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
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 1.0f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE,
        };

        VkPipelineColorBlendAttachmentState colorBlendAttachment
        {
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        };

        if (info.blendingMode == PipelineBlendType::Alpha)
        {
            colorBlendAttachment.blendEnable = VK_TRUE;
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        }
        else if (info.blendingMode == PipelineBlendType::None)
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
            .logicOpEnable = info.blendingMode == PipelineBlendType::Logical,
            .logicOp = VK_LOGIC_OP_XOR,
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
            .pDepthStencilState = nullptr,
            .pColorBlendState = &colorBlending,
            .pDynamicState = &dynamicState,
            .layout = info.layout,
            .renderPass = info.renderPass,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1,
        };

        VK_HANDLE_EXCEPT(vkCreateGraphicsPipelines(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
            VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, pipeline));
    }

    void BindShader(const Renderer* renderer, const Shader* shader)
    {
        vkCmdBindPipeline(((VKRenderer*)renderer->handle)->currentCommand, VK_PIPELINE_BIND_POINT_GRAPHICS,
            ((VkShader*)shader->handle)->pipeline);
    }
}