#include "hvk_texturesampler.h"
#include "hvk_graphics.h"

namespace hf
{
    VkTextureSampler::VkTextureSampler(const TextureSamplerDefinitionInfo& info)
    {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = (VkFilter)info.filter;
        samplerInfo.minFilter = (VkFilter)info.filter;
        samplerInfo.mipmapMode = (VkSamplerMipmapMode)info.mipMaps.mode;
        samplerInfo.addressModeU = (VkSamplerAddressMode)info.repeatMode;
        samplerInfo.addressModeV = (VkSamplerAddressMode)info.repeatMode;
        samplerInfo.addressModeW = (VkSamplerAddressMode)info.repeatMode;
        samplerInfo.mipLodBias = info.mipMaps.lodBias;
        samplerInfo.minLod = info.mipMaps.minLod;
        samplerInfo.maxLod = info.mipMaps.maxLod;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = !info.useNormalizedCoordinates;

        if (info.anisotropicFilter != TextureAnisotropicFilter::None &&
            GRAPHICS_DATA.device.features.features.samplerAnisotropy)
        {
            samplerInfo.anisotropyEnable = true;
            samplerInfo.maxAnisotropy = glm::min(
                GRAPHICS_DATA.device.properties.properties.limits.maxSamplerAnisotropy,
                (float)info.anisotropicFilter);
        }
        else
        {
            samplerInfo.anisotropyEnable = false;
            samplerInfo.maxAnisotropy = 1.0f;
        }

        if (info.comparison != ComparisonOperation::None)
        {
            samplerInfo.compareEnable = true;
            samplerInfo.compareOp = (VkCompareOp)((int32_t)info.comparison - 1);
        }
        else
        {
            samplerInfo.compareEnable = false;
            samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        }

        VK_HANDLE_EXCEPT(vkCreateSampler(GRAPHICS_DATA.device.logicalDevice.device,
            &samplerInfo, &GRAPHICS_DATA.platform.allocator, &sampler));
    }

    VkTextureSampler::~VkTextureSampler()
    {
        if (sampler) vkDestroySampler(GRAPHICS_DATA.device.logicalDevice.device, sampler, &GRAPHICS_DATA.platform.allocator);
    }

    bool IsValidSampler(TextureSampler sampler)
    {
        return sampler > 0 && sampler <= GRAPHICS_DATA.textureSamplers.size();
    }

    URef<VkTextureSampler>& GetSampler(TextureSampler sampler)
    {
        if (!IsValidSampler(sampler)) throw GENERIC_EXCEPT("[Hyperflow]", "Invalid sampler");
        return GRAPHICS_DATA.textureSamplers[sampler - 1];
    }
}