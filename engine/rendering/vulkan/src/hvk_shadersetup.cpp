#include "hvk_shadersetup.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    VkShaderSetup::VkShaderSetup(const ShaderSetupDefinitionInfo& info)
    {
        pushConstantSize = 0;
        stageFlags = 0;
        for (uint32_t i = 0; i < info.pushConstantCount; i++)
        {
            const auto& pf = info.pPushConstants[i];
            GRAPHICS_DATA.preAllocBuffers.pushConstantRanges[i] =
            {
                .stageFlags = (VkShaderStageFlags)pf.usageFlags,
                .offset = pf.offsetInBytes,
                .size = pf.sizeInBytes
            };
            pushConstantSize += pf.sizeInBytes;
            stageFlags |= (VkShaderStageFlags)pf.usageFlags;
        }

        if (pushConstantSize > 128) throw GENERIC_EXCEPT("[Hyperflow]", "Push constant size is too big, it should be under 128 bytes");

        VkPipelineLayoutCreateInfo pipelineLayoutInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pushConstantRangeCount = info.pushConstantCount,
            .pPushConstantRanges = GRAPHICS_DATA.preAllocBuffers.pushConstantRanges,
        };

        if (info.bufferCount > 0 || info.textureLayoutCount > 0)
        {
            for (uint32_t i = 0; i < info.bufferCount; i++)
            {
                auto& buffer = GetUniform(info.pBuffers[i]);
                GRAPHICS_DATA.preAllocBuffers.descLayouts[i] = buffer->layout;
            }

            for (uint32_t i = 0; i < info.textureLayoutCount; i++)
            {
                auto& layout = GetTextureLayout(info.pTextureLayouts[i]);
                GRAPHICS_DATA.preAllocBuffers.descLayouts[info.bufferCount + i] = layout->layout;
            }

            pipelineLayoutInfo.pSetLayouts = GRAPHICS_DATA.preAllocBuffers.descLayouts;
            pipelineLayoutInfo.setLayoutCount = info.bufferCount + info.textureLayoutCount;
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
        if (layout) vkDestroyPipelineLayout(GRAPHICS_DATA.defaultDevice->logicalDevice.device, layout, nullptr);
    }

    bool IsValidShaderSetup(ShaderSetup setup)
    {
        return setup > 0 && setup <= GRAPHICS_DATA.shaderSetups.size();
    }

    URef<VkShaderSetup>& GetShaderSetup(ShaderSetup setup)
    {
        if (!IsValidShaderSetup(setup)) throw GENERIC_EXCEPT("[Hyperflow]", "Invalid shader setup");
        return GRAPHICS_DATA.shaderSetups[setup - 1];
    }

    void BindShaderSetup(VkRenderer* rn, ShaderSetup setup)
    {
        rn->currentLayout = GetShaderSetup(setup)->layout;
    }

    void UploadPushConstants(const VkRenderer* rn, const PushConstantUploadInfo& info)
    {
        auto& shaderSetup = GetShaderSetup(info.shaderSetup);
        vkCmdPushConstants(rn->currentCommand, rn->currentLayout, shaderSetup->stageFlags,
            0, shaderSetup->pushConstantSize, info.data);
    }
}