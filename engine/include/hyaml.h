#ifndef HYAML_H
#define HYAML_H

#include <ryml.hpp>
#define RYML_STD_STRING
#include <ryml_std.hpp>
#include "hshared.h"

namespace hf
{
    bool ConvertToBool_i(const std::string_view& view);

    void ReadTextureDetails_i       (ryml::NodeRef root, TextureDetails& result);
    void ReadCubemapTexturePaths_i  (ryml::NodeRef root, CubemapTexturePaths& result);
    void ReadTextureMipMapInfo_i    (ryml::NodeRef root, TextureMipMapInfo& result);
    void ReadBiasOptions_i          (ryml::NodeRef root, ShaderDepthBiasOptions& result);
    void ReadRasterizerOptions_i    (ryml::NodeRef root, ShaderRasterizerOptions& result);
    void ReadDepthStencilOptions_i  (ryml::NodeRef root, ShaderDepthStencilOptions& result);
    void ReadBlendingOptions_i      (ryml::NodeRef root, BlendingOptions& result);
    void ReadShaderBlendingOptions_i(ryml::NodeRef root, ShaderBlendingOptions& result);

    void ReadBlendingOptions_i      (c4::yml::ConstNodeRef root, BlendingOptions& result);
    void ReadShaderBlendingOptions_i(c4::yml::ConstNodeRef root, ShaderBlendingOptions& result);
    void ReadAudioClipSettings_i    (c4::yml::ConstNodeRef root, AudioClipSettings& result);
}

#endif //HYAML_H
