#include "include/hvk_graphics.h"

namespace hf
{
    void CreatePipelineLayout(const VkPipelineLayoutCreationInfo& info, VkPipelineLayout* pipelineLayout)
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = info.layoutCount,
            .pSetLayouts = nullptr,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr,
        };

        VK_HANDLE_EXCEPT(vkCreatePipelineLayout(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
                         &pipelineLayoutInfo, nullptr, pipelineLayout));
    }

    void DestroyPipelineLayout(VkPipelineLayout* pipelineLayout)
    {
        if (*pipelineLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(GRAPHICS_DATA.defaultDevice->logicalDevice.device, *pipelineLayout, nullptr);
            *pipelineLayout = VK_NULL_HANDLE;
        }
    }
}