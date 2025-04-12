#include "hwindow.h"
#include "hshader.h"
#include "hyperflow.h"
#include "hvk_graphics.h"
#include "exceptions/hgraphicsexception.h"

namespace hf
{
    static bool CreateShaderModule(const Ref<Window>& window, const std::vector<char>& code, VkShaderModule* result);

    Shader::Shader(const Ref<Window>& window, const std::string& vertPath, const std::string& fragPath)
    {
        std::vector<char> vertexCode{}, fragmentCode{};
        utils::ReadFile(TO_RES_PATH(vertPath), vertexCode);
        utils::ReadFile(TO_RES_PATH(fragPath), fragmentCode);

        VkShaderModule vertModule{}, fragModule{};
        CreateShaderModule(window, vertexCode, &vertModule);
        CreateShaderModule(window, fragmentCode, &fragModule);

        VkPipelineShaderStageCreateInfo shaderStages[] =
        {
            {//vertex
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = vertModule,
                .pName = vertPath.c_str()
            },
            {//fragment
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = fragModule,
                .pName = fragPath.c_str()
            }
        };

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
            .pViewports = &((VKRendererData*)window->renderer->handle)->viewport,
            .scissorCount = 1,
            .pScissors = &((VKRendererData*)window->renderer->handle)->scissor,
        };

        VkPipelineRasterizationStateCreateInfo rasterizer
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .lineWidth = 1.0f,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
        };

        VkPipelineMultisampleStateCreateInfo multisampling
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .sampleShadingEnable = VK_FALSE,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .minSampleShading = 1.0f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE,
        };

        auto rendererData = (VKRendererData*)window->renderer->handle;
        vkDestroyShaderModule(rendererData->defaultDevice->logicalDevice.device, vertModule, nullptr);
        vkDestroyShaderModule(rendererData->defaultDevice->logicalDevice.device, fragModule, nullptr);
        isAlive = true;
    }

    Shader::~Shader()
    {

    }

    bool CreateShaderModule(const Ref<Window>& window, const std::vector<char>& shaderCode, VkShaderModule* result)
    {
        VkShaderModuleCreateInfo createInfo
        {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = shaderCode.size(),
            .pCode = (uint32_t*)shaderCode.data(),
        };
        auto rendererData = (VKRendererData*)window->renderer->handle;
        VK_HANDLE_EXCEPT(vkCreateShaderModule(rendererData->defaultDevice->logicalDevice.device,
            &createInfo, nullptr, result));

        return true;
    }
}