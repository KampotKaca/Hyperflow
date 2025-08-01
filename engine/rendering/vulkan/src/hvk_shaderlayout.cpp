#include "hvk_shaderlayout.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    VkShaderLayout::VkShaderLayout(const ShaderLayoutDefinitionInfo& info)
        : pushConstantInfo(info.pushConstant)
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

        VkPushConstantRange pushConstant{};
        if (info.pushConstant.usageFlags != ShaderUsageStage::None &&
            pushConstantInfo.sizeInBytes > 0)
        {
            pushConstant.stageFlags = (VkShaderStageFlags)pushConstantInfo.usageFlags;
            pushConstant.offset = 0;
            pushConstant.size = pushConstantInfo.sizeInBytes;

            pipelineLayoutInfo.pushConstantRangeCount = 1;
            pipelineLayoutInfo.pPushConstantRanges = &pushConstant;
        }
        else
        {
            pipelineLayoutInfo.pushConstantRangeCount = 0;
            pipelineLayoutInfo.pPushConstantRanges = nullptr;
        }

        if (info.bufferCount > 0 || info.textureLayoutCount > 0)
        {
            GRAPHICS_DATA.preAllocBuffers.descLayouts.resize(info.bufferCount + info.textureLayoutCount);
            GRAPHICS_DATA.preAllocBuffers.descLayouts.clear();

            for (uint32_t i = 0; i < info.bufferCount; i++)
                GRAPHICS_DATA.preAllocBuffers.descLayouts.push_back(GetBuffer(info.pBuffers[i])->layout);

            for (uint32_t i = 0; i < info.textureLayoutCount; i++)
                GRAPHICS_DATA.preAllocBuffers.descLayouts.push_back(GetTextureLayout(info.pTextureLayouts[i])->layout);

            pipelineLayoutInfo.pSetLayouts = GRAPHICS_DATA.preAllocBuffers.descLayouts.data();
            pipelineLayoutInfo.setLayoutCount = GRAPHICS_DATA.preAllocBuffers.descLayouts.size();
        }
        else
        {
            pipelineLayoutInfo.pSetLayouts = nullptr;
            pipelineLayoutInfo.setLayoutCount = 0;
        }

        VK_HANDLE_EXCEPT(vkCreatePipelineLayout(GRAPHICS_DATA.device.logicalDevice.device,
                         &pipelineLayoutInfo, &GRAPHICS_DATA.platform.allocator, &layout));
    }

    VkShaderLayout::~VkShaderLayout()
    {
        if (layout) vkDestroyPipelineLayout(GRAPHICS_DATA.device.logicalDevice.device, layout, &GRAPHICS_DATA.platform.allocator);
    }

    bool IsValidShaderLayout(ShaderLayout setup)
    {
        return setup > 0 && setup <= GRAPHICS_DATA.shaderLayouts.size();
    }

    URef<VkShaderLayout>& GetShaderLayout(ShaderLayout setup)
    {
        if (!IsValidShaderLayout(setup)) throw GENERIC_EXCEPT("[Hyperflow]", "Invalid shader setup");
        return GRAPHICS_DATA.shaderLayouts[setup - 1];
    }

    void BindShaderLayout(VkRenderer* rn, ShaderLayout setup)
    {
        rn->currentLayout = GetShaderLayout(setup)->layout;
    }

    void UploadPushConstants(const VkRenderer* rn, const PushConstantUploadInfo& info)
    {
        const auto& shaderLayout = GetShaderLayout(info.layout);
        vkCmdPushConstants(rn->currentCommand, rn->currentLayout, (VkShaderStageFlags)shaderLayout->pushConstantInfo.usageFlags,
            0, shaderLayout->pushConstantInfo.sizeInBytes, info.data);
    }
}