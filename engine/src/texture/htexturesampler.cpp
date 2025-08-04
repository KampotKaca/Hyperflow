#include "hyperflow.h"
#include "hyaml.h"
#include "hstrconversion.h"

namespace hf
{
    TextureSampler Define(const TextureSamplerDefinitionInfo& info) { return inter::HF.renderingApi.api.DefineTextureSampler(info); }

    TextureSampler DefineTextureSamplerAsset(const char* assetPath)
    {
        const auto assetLoc = TO_RES_PATH(std::string("texturesamplers/") + assetPath) + ".meta";
        std::vector<char> metadata{};
        if (!START_READING(assetLoc.c_str(), metadata)) return 0;

        try
        {
            TextureSamplerDefinitionInfo info{};

            ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
            ryml::NodeRef root = tree.rootref();

            {
                info.anisotropicFilter = (TextureAnisotropicFilter)std::stoi(root["anisotropicFilter"].val().str);

                {
                    const auto useNormalizedCoordinates = root["useNormalizedCoordinates"].val();
                    const std::string_view vView{useNormalizedCoordinates.str, useNormalizedCoordinates.len};
                    info.useNormalizedCoordinates = ConvertToBool_i(vView);
                }

                {
                    const auto usageFlags = root["filter"].val();
                    const std::string_view vView{usageFlags.str, usageFlags.len};
                    info.filter = STRING_TO_TEXTURE_FILTER(vView);
                }

                {
                    const auto repeatMode = root["repeatMode"].val();
                    const std::string_view vView{repeatMode.str, repeatMode.len};
                    info.repeatMode = STRING_TO_TEXTURE_REPEAT_MODE(vView);
                }

                {
                    const auto comparison = root["comparison"].val();
                    const std::string_view vView{comparison.str, comparison.len};
                    info.comparison = STRING_TO_COMPARISON_OPERATION(vView);
                }

                ReadTextureMipMapInfo_i(root["mipMaps"], info.mipMaps);
            }

            const auto sampler = Define(info);
            inter::HF.graphicsResources.textureSamplers[assetPath] = sampler;
            return sampler;
        }catch (...)
        {
            LOG_ERROR("[Hyperflow] Error parsing Texture Sampler: %s", assetPath);
            return 0;
        }
    }
}