#include "hvk_texturelayout.h"
#include "hvk_shared.h"
#include "hvk_graphics.h"

namespace hf
{
    VkTextureLayout::VkTextureLayout(const TextureLayoutDefinitionInfo& info)
    {
        bindingInfos = List<TextureLayoutBindingInfo>(info.bindingCount);
        memcpy(bindingInfos.data(), info.pBindings, sizeof(TextureLayoutBindingInfo) * info.bindingCount);

        GRAPHICS_DATA.preAllocBuffers.descLayoutBindings.resize(info.bindingCount);
        GRAPHICS_DATA.preAllocBuffers.descLayoutBindings.clear();

        for (uint32_t i = 0; i < info.bindingCount; ++i)
        {
            auto& bindingInfo = info.pBindings[i];
            VkDescriptorSetLayoutBinding setLayoutBinding{};
            setLayoutBinding.binding = bindingInfo.bindingId;
            setLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            setLayoutBinding.descriptorCount = bindingInfo.arraySize;
            setLayoutBinding.stageFlags = (VkShaderStageFlags)bindingInfo.usageFlags;
            GRAPHICS_DATA.preAllocBuffers.descLayoutBindings.push_back(setLayoutBinding);
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};

        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = GRAPHICS_DATA.preAllocBuffers.descLayoutBindings.size();
        layoutInfo.pBindings = GRAPHICS_DATA.preAllocBuffers.descLayoutBindings.data();
        layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_DESCRIPTOR_BUFFER_BIT_EXT;

        VK_HANDLE_EXCEPT(vkCreateDescriptorSetLayout(GRAPHICS_DATA.device.logicalDevice.device,
            &layoutInfo, &GRAPHICS_DATA.platform.allocator, &layout));
    }

    VkTextureLayout::~VkTextureLayout()
    {
        if (layout) vkDestroyDescriptorSetLayout(GRAPHICS_DATA.device.logicalDevice.device, layout, &GRAPHICS_DATA.platform.allocator);
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