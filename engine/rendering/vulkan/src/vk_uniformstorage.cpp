#include "hvk_uniformstorage.h"
#include "hvk_uniformbuffer.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    VkUniformStorage::VkUniformStorage(const UniformStorageDefinitionInfo& info)
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr,
        };

        std::vector<VkDescriptorSetLayout> layouts(info.bufferCount);

        if (info.bufferCount > 0)
        {
            for (uint32_t i = 0; i < info.bufferCount; i++)
            {
                auto& buffer = GetUniform(info.pBuffers[i]);
                layouts[i] = buffer.layout;
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

    VkUniformStorage::~VkUniformStorage()
    {
        vkDestroyPipelineLayout(GRAPHICS_DATA.defaultDevice->logicalDevice.device, layout, nullptr);
    }

    bool IsValidStorage(UniformStorage storage)
    {
        return storage > 0 && storage <= GRAPHICS_DATA.uniformStorages.size();
    }

    const VkUniformStorage& GetStorage(UniformStorage storage)
    {
        if (!IsValidStorage(storage)) throw GENERIC_EXCEPT("[Hyperflow]", "Invalid uniform storage");
        return GRAPHICS_DATA.uniformStorages[storage - 1];
    }

    void BindUniformStorage(VKRenderer* rn, UniformStorage storage)
    {
        rn->currentLayout = GetStorage(storage).layout;
    }
}
