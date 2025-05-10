#include "hvk_shadersetup.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    VkShaderSetup::VkShaderSetup(const ShaderSetupDefinitionInfo& info)
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr,
        };

        std::vector<VkDescriptorSetLayout> layouts(info.bufferCount + info.textureLayoutCount);

        if (info.bufferCount > 0 || info.textureLayoutCount > 0)
        {
            for (uint32_t i = 0; i < info.bufferCount; i++)
            {
                auto& buffer = GetUniform(info.pBuffers[i]);
                layouts[i] = buffer.layout;
            }

            for (uint32_t i = 0; i < info.textureLayoutCount; i++)
            {
                auto& layout = GetTextureLayout(info.pTextureLayouts[i]);
                layouts[info.bufferCount + i] = layout.layout;
            }

            pipelineLayoutInfo.pSetLayouts = layouts.data();
            pipelineLayoutInfo.setLayoutCount = (uint32_t)layouts.size();
        }
        else
        {
            pipelineLayoutInfo.pSetLayouts = nullptr;
            pipelineLayoutInfo.setLayoutCount = 0;
        }

        VK_HANDLE_EXCEPT(vkCreatePipelineLayout(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
                         &pipelineLayoutInfo, nullptr, &layout));
    }

    VkShaderSetup::~VkShaderSetup()
    {
        vkDestroyPipelineLayout(GRAPHICS_DATA.defaultDevice->logicalDevice.device, layout, nullptr);
    }

    bool IsValidShaderSetup(ShaderSetup setup)
    {
        return setup > 0 && setup <= GRAPHICS_DATA.shaderSetups.size();
    }

    const VkShaderSetup& GetShaderSetup(ShaderSetup setup)
    {
        if (!IsValidShaderSetup(setup)) throw GENERIC_EXCEPT("[Hyperflow]", "Invalid shader setup");
        return GRAPHICS_DATA.shaderSetups[setup - 1];
    }

    void BindShaderSetup(VkRenderer* rn, ShaderSetup setup)
    {
        rn->currentLayout = GetShaderSetup(setup).layout;
    }
}