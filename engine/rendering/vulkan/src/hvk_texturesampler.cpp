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
            .addressModeU = (VkSamplerAddressMode)info.repeatMode,
            .addressModeV = (VkSamplerAddressMode)info.repeatMode,
            .addressModeW = (VkSamplerAddressMode)info.repeatMode,
            .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
            .unnormalizedCoordinates = !info.useNormalizedCoordinates,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .mipLodBias = 0.0f,
            .minLod = 0.0f,
            .maxLod = 0.0f,
        };

        if (info.anisotropicFilter != TextureAnisotropicFilter::None &&
            GRAPHICS_DATA.defaultDevice->features.samplerAnisotropy)
        {
            samplerInfo.anisotropyEnable = true;
            samplerInfo.maxAnisotropy = glm::min(
                GRAPHICS_DATA.defaultDevice->properties.limits.maxSamplerAnisotropy,
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

        VK_HANDLE_EXCEPT(vkCreateSampler(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
            &samplerInfo, nullptr, &sampler));
    }

    VkTextureSampler::~VkTextureSampler()
    {
        vkDestroySampler(GRAPHICS_DATA.defaultDevice->logicalDevice.device, sampler, nullptr);
    }

    bool IsValidSampler(TextureSampler sampler)
    {
        return sampler > 0 && sampler <= GRAPHICS_DATA.textureSamplers.size();
    }

    const VkTextureSampler& GetSampler(TextureSampler sampler)
    {
        if (!IsValidSampler(sampler)) throw GENERIC_EXCEPT("[Hyperflow]", "Invalid sampler");
        return GRAPHICS_DATA.textureSamplers[sampler - 1];
    }
}