#ifndef HYAML_H
#define HYAML_H

#include <ryml.hpp>
#define RYML_STD_STRING
#include <ryml_std.hpp>
#include "hshared.h"

namespace hf
{
    bool ConvertToBool_i(const std::string_view& view);
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureRepeatMode& result);
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureChannel& result);
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureFilter& result);
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureFormat& result);
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureTiling& result);
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, BufferMemoryType& result);
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureResultLayoutType& result);
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, ShaderCullMode& result);
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, ShaderFaceDirection& result);
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, MeshPolygonMode& result);
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, ColorBlendFactorType& result);
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, MipMapMode& result);

    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureAnisotropicFilter& result);
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, uint32_t& result);
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, float_t& result);
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, bool& result);

    bool YamlGetIfArray(ryml::NodeRef root, c4::csubstr key, TextureAspectFlags& result);
    bool YamlGetIfArray(ryml::NodeRef root, c4::csubstr key, TextureUsageFlags& result);

    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureDetails& result);

    bool ReadTextureDetails_i       (ryml::NodeRef root, TextureDetails& result);
    bool ReadCubemapTexturePaths_i  (ryml::NodeRef root, CubemapTexturePaths& result);
    bool ReadTextureMipMapInfo_i    (ryml::NodeRef root, TextureMipMapInfo& result);
    void ReadBiasOptions_i          (ryml::NodeRef root, ShaderDepthBiasOptions& result);
    void ReadRasterizerOptions_i    (ryml::NodeRef root, ShaderRasterizerOptions& result);
    void ReadDepthStencilOptions_i  (ryml::NodeRef root, ShaderDepthStencilOptions& result);
    void ReadBlendingOptions_i      (ryml::NodeRef root, BlendingOptions& result);
    void ReadShaderBlendingOptions_i(ryml::NodeRef root, ShaderBlendingOptions& result);

    void ReadBlendingOptions_i      (c4::yml::ConstNodeRef root, BlendingOptions& result);
    void ReadShaderBlendingOptions_i(c4::yml::ConstNodeRef root, ShaderBlendingOptions& result);
    void ReadShaderModulesInfo_i    (c4::yml::ConstNodeRef root, const Ref<ShaderLibrary>& lib, ShaderModulesInfo& result);
    void ReadAudioClipSettings_i    (c4::yml::ConstNodeRef root, AudioClipSettings& result);
}

#endif //HYAML_H
