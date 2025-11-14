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

        if (!YamlGetIf_i(root, "anisotropicFilter", info.anisotropicFilter)) log_warn_s("[Hyperflow] Texture Sampler %s has invalid anisotropicFilter", assetPath);
        if (!YamlGetIf_i(root, "useNormalizedCoordinates", info.useNormalizedCoordinates)) log_warn_s("[Hyperflow] Texture Sampler %s has invalid useNormalizedCoordinates", assetPath);
        if (!YamlGetIf_i(root, "filter", info.filter)) log_warn_s("[Hyperflow] Texture Sampler %s has invalid filter", assetPath);
        if (!YamlGetIf_i(root, "repeatMode", info.repeatMode)) log_warn_s("[Hyperflow] Texture Sampler %s has invalid repeatMode", assetPath);
        if (!YamlGetIf_i(root, "comparison", info.comparison)) log_warn_s("[Hyperflow] Texture Sampler %s has invalid comparison", assetPath);
        if (!YamlGetIf_i(root, "mipMaps", info.mipMaps)) log_warn_s("[Hyperflow] Texture Sampler %s has invalid mipMaps", assetPath);

        const auto sampler = Define(info);
        ir::HF.graphicsResources.textureSamplers[assetPath] = sampler;
        return sampler;
    }
}