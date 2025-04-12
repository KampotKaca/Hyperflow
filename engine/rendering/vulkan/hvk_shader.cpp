#include "hvkshader.h"
#include "hshader.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "hvk_graphics.h"
#include "exceptions/hgraphicsexception.h"

namespace hf::inter::rendering
{
    static bool CreateShaderModule(const char* code, uint32_t codeSize, VkShaderModule* result);

    void* CreateShader(const ShaderCreationInfo& info, const Shader* shader)
    {
        auto vShader = new VkShader
        {
            .shader = shader
        };

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
                .pName = shader->vPath.c_str(),
            },
            {//fragment
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = fragModule,
                .pName = shader->fPath.c_str()
            }
        };

        auto& device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        vkDestroyShaderModule(device, vertModule, nullptr);
        vkDestroyShaderModule(device, fragModule, nullptr);
        return vShader;
    }

    void DestroyShader(void* shader)
    {
        auto sh = (VkShader*)shader;
        delete(sh);
    }

    bool CreateShaderModule(const char* code, uint32_t codeSize, VkShaderModule* result)
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

        return true;
    }
}