#ifndef HVK_TEXTURESAMPLER_H
#define HVK_TEXTURESAMPLER_H

#include "hvk_shared.h"

namespace hf
{
    struct VkTextureSampler
    {
        explicit VkTextureSampler(const TextureSamplerDefinitionInfo& info);
        ~VkTextureSampler();

        VkSampler sampler{};
    };

    bool IsValidSampler(TextureSampler sampler);
    const VkTextureSampler& GetSampler(TextureSampler sampler);
}

#endif //HVK_TEXTURESAMPLER_H
