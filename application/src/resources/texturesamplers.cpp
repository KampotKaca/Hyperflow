#include "resources/texturesamplers.h"

namespace app
{
    AppTextureSamplers APP_TEXTURE_SAMPLERS;

    void TextureSamplerDefineAll()
    {
        hf::TextureSamplerDefinitionInfo defaultSamplerInfo
        {
            .filter = hf::TextureFilter::Bilinear,
            .anisotropicFilter = hf::TextureAnisotropicFilter::X16,
            .repeatMode = hf::TextureRepeatMode::Repeat,
            .useNormalizedCoordinates = true,
            .comparison = hf::ComparisonOperation::None,
            .mipMaps =
            {
                .mode = hf::MipMapMode::Linear,
                .minLod = 0.0f,
                .maxLod = 8.0f,
                .lodBias = 0.0f
            }
        };
        APP_TEXTURE_SAMPLERS.defaultSampler = hf::Define(defaultSamplerInfo);
    }
}