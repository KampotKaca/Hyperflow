#include "hyperflow.h"
#include "hyaml.h"
#include "hstrconversion.h"
#include "hinternal.h"

namespace hf
{
    TextureSampler Define(const TextureSamplerDefinitionInfo& info) { return ir::HF.renderingApi.api.DefineTextureSampler(info); }

    TextureSampler DefineTextureSamplerAsset(const char* assetPath)
    {
        const auto assetLoc = TO_RES_PATH(std::string("texturesamplers/") + assetPath) + ".meta";
        List<char> metadata{};
        START_READING(assetLoc.c_str(), metadata);

        TextureSamplerDefinitionInfo info{};

        ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
        ryml::NodeRef root = tree.rootref();

        {
            if (!YamlGetIf(root, "anisotropicFilter", info.anisotropicFilter)) log_warn_s("[Hyperflow] Texture Sampler %s has invalid anisotropicFilter", assetPath);
            if (!YamlGetIf(root, "useNormalizedCoordinates", info.useNormalizedCoordinates)) log_warn_s("[Hyperflow] Texture Sampler %s has invalid useNormalizedCoordinates", assetPath);
            if (!YamlGetIf(root, "filter", info.filter)) log_warn_s("[Hyperflow] Texture Sampler %s has invalid filter", assetPath);
            if (!YamlGetIf(root, "repeatMode", info.repeatMode)) log_warn_s("[Hyperflow] Texture Sampler %s has invalid repeatMode", assetPath);

            {
                auto node = root["comparison"];
                if (node.readable())
                {
                    const auto comparison = node.val();
                    const std::string_view vView{comparison.str, comparison.len};
                    info.comparison = STRING_TO_COMPARISON_OPERATION(vView);
                }
                else log_warn_s("[Hyperflow] TextureSampler '%s' has invalid comparison!", assetPath);
            }

            {
                auto node = root["mipMaps"];
                if (!node.readable() || !ReadTextureMipMapInfo_i(node, info.mipMaps))
                    log_warn_s("[Hyperflow] TextureSampler '%s' has invalid mipMaps!", assetPath);
            }
        }

        const auto sampler = Define(info);
        ir::HF.graphicsResources.textureSamplers[assetPath] = sampler;
        return sampler;
    }
}