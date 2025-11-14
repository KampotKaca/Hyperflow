#ifndef HYAML_H
#define HYAML_H

#include <ryml.hpp>
#define RYML_STD_STRING
#include <ryml_std.hpp>
#include "hshared.h"

namespace hf
{
    bool ConvertToBool_i(const std::string_view& view);

    template<typename T> bool YamlGetIf(T root, c4::csubstr key, TextureRepeatMode& result);
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, TextureChannel& result);
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, TextureFilter& result);
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, TextureFormat& result);
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, TextureTiling& result);
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, BufferMemoryType& result);
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, TextureResultLayoutType& result);
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, ShaderCullMode& result);
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, ShaderFaceDirection& result);
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, MeshPolygonMode& result);
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, ColorBlendFactorType& result);
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, MipMapMode& result);
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, ColorBlendOp& result);
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, ColorBlendFactorType& result);
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, DepthComparisonFunction& result);
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, StencilOperation& result);

    template<typename T> bool YamlGetIf(T root, c4::csubstr key, AudioClipFormat& result);
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, AudioClipChannelMixMode& result);
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, AudioClipDitherMode& result);

    template<typename T> bool YamlGetIf(T root, c4::csubstr key, TextureAnisotropicFilter& result);
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, uint32_t& result);
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, float_t& result);
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, bool& result);

    template<typename T> bool YamlGetIfArray(T root, c4::csubstr key, TextureAspectFlags& result);
    template<typename T> bool YamlGetIfArray(T root, c4::csubstr key, TextureUsageFlags& result);

    template<typename T> bool YamlGetIf(T root, c4::csubstr key, TextureDetails& result);
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, BlendingOptions& result);

    template<typename T> bool ReadTextureDetails_i       (T root, TextureDetails& result);
    template<typename T> bool ReadCubemapTexturePaths_i  (T root, CubemapTexturePaths& result);
    template<typename T> bool ReadTextureMipMapInfo_i    (T root, TextureMipMapInfo& result);
    template<typename T> bool ReadBiasOptions_i          (T root, ShaderDepthBiasOptions& result);
    template<typename T> bool ReadRasterizerOptions_i    (T oot, ShaderRasterizerOptions& result);
    template<typename T> bool ReadDepthStencilOptions_i  (T root, ShaderDepthStencilOptions& result);
    template<typename T> bool ReadBlendingOptions_i      (T root, BlendingOptions& result);
    template<typename T> bool ReadShaderBlendingOptions_i(T root, ShaderBlendingOptions& result);

    template<typename T> bool ReadShaderModulesInfo_i    (T root, const Ref<ShaderLibrary>& lib, ShaderModulesInfo& result);
    template<typename T> bool ReadAudioClipSettings_i    (T root, AudioClipSettings& result);
}

#endif //HYAML_H
