#include "hvk_texturelayout.h"
#include "hvk_shared.h"
#include "hvk_graphics.h"

namespace hf
{
    VkTextureLayout::VkTextureLayout(const TextureLayoutDefinitionInfo& info)
    {
        bindingInfos = std::vector<TextureLayoutBindingInfo>(info.bindingCount);
        memcpy(bindingInfos.data(), info.pBindings, sizeof(TextureLayoutBindingInfo) * info.bindingCount);

        for (uint32_t i = 0; i < info.bindingCount; ++i)
        {
            auto& bindingInfo = info.pBindings[i];
            GRAPHICS_DATA.preAllocBuffers.descLayoutBindings[i] =
            {
                .binding = bindingInfo.bindingId,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = bindingInfo.arraySize,
                .stageFlags = (VkShaderStageFlags)bindingInfo.usageFlags,
                .pImmutableSamplers = nullptr
            };
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = info.bindingCount,
            .pBindings = GRAPHICS_DATA.preAllocBuffers.descLayoutBindings,
        };

        VkDescriptorSetLayoutSupportKHR supportInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_SUPPORT
        };
        GRAPHICS_DATA.extensionFunctions.vkGetDescriptorSetLayoutSupportKHR(GRAPHICS_DATA.device.logicalDevice.device, &layoutInfo, &supportInfo);

        if (supportInfo.supported == VK_FALSE) throw GENERIC_EXCEPT("[Hyperflow]", "Texture layout not supported");

        VK_HANDLE_EXCEPT(vkCreateDescriptorSetLayout(GRAPHICS_DATA.device.logicalDevice.device,
            &layoutInfo, nullptr, &layout));
    }

    VkTextureLayout::~VkTextureLayout()
    {
        if (layout) vkDestroyDescriptorSetLayout(GRAPHICS_DATA.device.logicalDevice.device, layout, nullptr);
    }

    bool IsValidLayout(TextureLayout layout)
    {
        return layout > 0 && layout <= GRAPHICS_DATA.textureLayouts.size();
    }

    URef<VkTextureLayout>& GetTextureLayout(TextureLayout layout)
    {
        if (!IsValidLayout(layout)) throw GENERIC_EXCEPT("[Hyperflow]", "Invalid texture layout");
        return GRAPHICS_DATA.textureLayouts[layout - 1];
    }
}