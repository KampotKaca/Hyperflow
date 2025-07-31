#include "hvk_texturesampler.h"
#include "hvk_graphics.h"

namespace hf
{
    VkTextureSampler::VkTextureSampler(const TextureSamplerDefinitionInfo& info)
    {
        VkSamplerCreateInfo samplerInfo
        {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = (VkFilter)info.filter,
            .minFilter = (VkFilter)info.filter,
            .mipmapMode = (VkSamplerMipmapMode)info.mipMaps.mode,
            .addressModeU = (VkSamplerAddressMode)info.repeatMode,
            .addressModeV = (VkSamplerAddressMode)info.repeatMode,
            .addressModeW = (VkSamplerAddressMode)info.repeatMode,
            .mipLodBias = info.mipMaps.lodBias,
            .minLod = info.mipMaps.minLod,
            .maxLod = info.mipMaps.maxLod,
            .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
            .unnormalizedCoordinates = !info.useNormalizedCoordinates,
        };

        if (info.anisotropicFilter != TextureAnisotropicFilter::None &&
            GRAPHICS_DATA.device.features.samplerAnisotropy)
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