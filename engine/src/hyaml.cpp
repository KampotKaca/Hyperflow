#include "hstrconversion.h"
#include "hyaml.h"

namespace hf
{
    //region Implementations
    bool ConvertToBool_i(const std::string_view& view)
    {
        std::string lowerStr(view);
        std::ranges::transform(lowerStr, lowerStr.begin(), tolower);

        return lowerStr == "true" || lowerStr == "1" || lowerStr == "yes";
    }

#define YAML_GET(func)\
    auto node = root[key];\
    if (node.readable())\
    {\
        const auto v = node.val();\
        const std::string_view vView{v.str, v.len};\
        result = func(vView);\
        return true;\
    }\
    return false

#define YAML_GET_S(func)\
    auto node = root[key];\
    if (node.readable())\
    {\
        result = func(node.val().str);\
        return true;\
    }\
    return false

#define YAML_GET_ARRAY(func)\
    auto node = root[key];\
    if (node.readable() && node.num_children() > 0)\
    {\
        for (auto fmt : node.children())\
        {\
            if (fmt.readable())\
            {\
                const auto v = fmt.val();\
                const std::string_view vView{v.str, v.len};\
                result |= func(vView);\
            }\
            else return false;\
        }\
        return true;\
    }\
    else return false

#define DEF(v)\
    if (!YamlGetIf(root, #v, result.v))\
    {\
        log_warn_s("[Hyperflow] Unable to retrieve %s", #v);\
        res = false;\
    }

#define DEF_ARR(v)\
    if (!YamlGetIfArray(root, #v, result.v))\
    {\
        log_warn_s("[Hyperflow] Unable to retrieve %s", #v);\
        res = false;\
    }

#define DEF_GET(n)\
    if (!root.get_if(#n, &result.n))\
    {\
        log_error_s("[Hyperflow] Unable to retrieve %s", #n);\
        res = false;\
    }

    template<typename T> bool YamlGetIf(T root, c4::csubstr key, TextureRepeatMode& result) { YAML_GET(STRING_TO_TEXTURE_REPEAT_MODE); }
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, TextureChannel& result) { YAML_GET(STRING_TO_TEXTURE_CHANNEL); }
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, TextureFilter& result) { YAML_GET(STRING_TO_TEXTURE_FILTER); }
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, TextureFormat& result) { YAML_GET(STRING_TO_TEXTURE_FORMAT); }
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, TextureTiling& result) { YAML_GET(STRING_TO_TEXTURE_TILING); }
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, BufferMemoryType& result) { YAML_GET(STRING_TO_BUFFER_MEMORY_TYPE); }
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, TextureResultLayoutType& result) { YAML_GET(STRING_TO_TEXTURE_RESULT_LAYOUT_TYPE); }
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, ShaderCullMode& result) { YAML_GET(STRING_TO_SHADER_CULL_MODE); }
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, ShaderFaceDirection& result) { YAML_GET(STRING_TO_SHADER_FACE_DIRECTION); }
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, MeshPolygonMode& result) { YAML_GET(STRING_TO_MESH_POLYGON_MODE); }
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, ColorBlendFactorType& result) { YAML_GET(STRING_TO_COLOR_BLEND_FACTOR_TYPE); }
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, MipMapMode& result) { YAML_GET(STRING_TO_MIP_MAP_MODE); }
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, ColorBlendOp& result) { YAML_GET(STRING_TO_COLOR_BLEND_OPERATION); }
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, DepthComparisonFunction& result) { YAML_GET(STRING_TO_DEPTH_COMPARISON_FUNCTION); }
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, StencilOperation& result) { YAML_GET(STRING_TO_STENCIL_OPERATION); }

    template<typename T> bool YamlGetIf(T root, c4::csubstr key, AudioClipFormat& result) { YAML_GET(STRING_TO_AUDIO_CLIP_FORMAT); }
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, AudioClipChannelMixMode& result) { YAML_GET(STRING_TO_AUDIO_CLIP_CHANNEL_MIX_MODE); }
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, AudioClipDitherMode& result) { YAML_GET(STRING_TO_AUDIO_CLIP_DITHER_MODE); }

    template<typename T> bool YamlGetIf(T root, c4::csubstr key, TextureAnisotropicFilter& result) { YAML_GET_S((TextureAnisotropicFilter)std::stoi); }
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, uint32_t& result) { YAML_GET_S((uint32_t)std::stoi); }
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, float_t& result) { YAML_GET_S(std::stof); }
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, bool& result) { YAML_GET(ConvertToBool_i); }

    template<typename T> bool YamlGetIfArray(T root, c4::csubstr key, TextureAspectFlags& result) { YAML_GET_ARRAY(STRING_TO_TEXTURE_ASPECT_FLAGS); }
    template<typename T> bool YamlGetIfArray(T root, c4::csubstr key, TextureUsageFlags& result) { YAML_GET_ARRAY(STRING_TO_TEXTURE_USAGE_FLAGS); }

    template<typename T> bool YamlGetIf(T root, c4::csubstr key, TextureDetails& result)
    {
        auto node = root[key];
        return node.readable() && ReadTextureDetails_i(node, result);
    }
    template<typename T> bool YamlGetIf(T root, c4::csubstr key, BlendingOptions& result)
    {
        auto node = root[key];
        if (node.readable())
        {
            ReadBlendingOptions_i(node, result);
            return true;
        }
        return false;
    }

    template<typename T> bool ReadTextureDetails_i(T root, TextureDetails& result)
    {
        bool res = true;
        DEF(format)
        DEF(tiling)
        DEF(memoryType)
        DEF(finalLayout)
        DEF_ARR(aspectFlags)
        DEF_ARR(usageFlags)

        return res;
    }
    template<typename T> bool ReadCubemapTexturePaths_i(T root, CubemapTexturePaths& result)
    {
        bool res = true;
        DEF_GET(left)
        DEF_GET(right)
        DEF_GET(down)
        DEF_GET(up)
        DEF_GET(back)
        DEF_GET(front)

        return res;
    }
    template<typename T> bool ReadTextureMipMapInfo_i(T root, TextureMipMapInfo& result)
    {
        bool res = true;
        DEF(mode)
        DEF(minLod)
        DEF(maxLod)
        DEF(lodBias)

        return res;
    }
    template<typename T> bool ReadBiasOptions_i(T root, ShaderDepthBiasOptions& result)
    {
        bool res = true;
        DEF(constantFactor)
        DEF(clamp)
        DEF(slopeFactor)
        return res;
    }
    template<typename T> bool ReadRasterizerOptions_i(T root, ShaderRasterizerOptions& result)
    {
        bool res = true;
        DEF(cullMode)
        DEF(faceDirection)
        DEF(polygonMode)
        DEF(enableRasterizerDiscard)
        DEF(enableDepthClamping)
        DEF(lineWidth)

        {
            auto node = root["biasOptions"];
            if (node.readable())
            {
                ShaderDepthBiasOptions biasOptions{};
                ReadBiasOptions_i(node, biasOptions);
                result.biasOptions = biasOptions;
            }
        }

        return res;
    }
    template<typename T> bool ReadBlendingOptions_i(T root, BlendingOptions& result)
    {
        bool res = true;
        DEF(srcFactor)
        DEF(dstFactor)
        DEF(op)
        return res;
    }
    template<typename T> bool ReadShaderBlendingOptions_i(T root, ShaderBlendingOptions& result)
    {
        bool res = true;
        DEF(color)
        DEF(alpha)
        return res;
    }
    template<typename T> bool ReadShaderModulesInfo_i(T root, const Ref<ShaderLibrary>& lib, ShaderModulesInfo& result)
    {
#define MODULE_DEF(l, func)\
        {\
            auto node = root[#l];\
            if (node.readable())\
            {\
                const auto v = node.val();\
                const std::string_view vView{v.str, v.len};\
                result.l = func(lib, vView);\
            }\
            else\
            {\
                res = false;\
                log_warn_s("[Hyperflow] Unable to retrive %s", #l);\
            }\
        }

        bool res = true;
        MODULE_DEF(vertexInputModuleId, GetVertexInputModule)
        MODULE_DEF(preRasterModuleId, GetPreRasterModule)
        MODULE_DEF(fragmentModuleId, GetFragmentModule)
        MODULE_DEF(fragmentOutputModuleId, GetFragmentOutputModule)
#undef MODULE_DEF
        return res;
    }
    template<typename T> bool ReadDepthStencilOptions_i(T root, ShaderDepthStencilOptions& result)
    {
        bool res = true;
        DEF(enableDepth)
        DEF(writeDepth)
        DEF(comparisonFunc)
        DEF(enableDepthBounds)
        DEF(enableStencil)
        DEF(frontStencil)
        DEF(backStencil)
        DEF(minDepthBounds)
        DEF(maxDepthBounds)
        return res;
    }
    template<typename T> bool ReadAudioClipSettings_i(T root, AudioClipSettings& result)
    {
        bool res = true;

        DEF(sampleRate)
        DEF(format)
        DEF(channelMixMode)
        DEF(ditherMode)
        return res;
    }

#undef YAML_GET
#undef YAML_GET_S
#undef YAML_GET_ARRAY

#undef DEF
#undef DEF_ARR
#undef DEF_GET
    //endregion
    //region NodeRef

    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureRepeatMode& result);
    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureChannel& result);
    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureFilter& result);
    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureFormat& result);
    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureTiling& result);
    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, BufferMemoryType& result);
    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureResultLayoutType& result);
    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, ShaderCullMode& result);
    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, ShaderFaceDirection& result);
    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, MeshPolygonMode& result);
    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, ColorBlendFactorType& result);
    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, MipMapMode& result);
    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, ColorBlendOp& result);
    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, DepthComparisonFunction& result);
    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, StencilOperation& result);

    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, AudioClipFormat& result);
    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, AudioClipChannelMixMode& result);
    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, AudioClipDitherMode& result);

    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureAnisotropicFilter& result);
    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, uint32_t& result);
    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, float_t& result);
    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, bool& result);

    template bool YamlGetIfArray(ryml::NodeRef root, c4::csubstr key, TextureAspectFlags& result);
    template bool YamlGetIfArray(ryml::NodeRef root, c4::csubstr key, TextureUsageFlags& result);

    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureDetails& result);
    template bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, BlendingOptions& result);

    template bool ReadTextureDetails_i(ryml::NodeRef root, TextureDetails& result);
    template bool ReadCubemapTexturePaths_i(ryml::NodeRef root, CubemapTexturePaths& result);
    template bool ReadTextureMipMapInfo_i(ryml::NodeRef root, TextureMipMapInfo& result);
    template bool ReadBiasOptions_i(ryml::NodeRef root, ShaderDepthBiasOptions& result);
    template bool ReadRasterizerOptions_i(ryml::NodeRef root, ShaderRasterizerOptions& result);
    template bool ReadBlendingOptions_i(ryml::NodeRef root, BlendingOptions& result);
    template bool ReadShaderBlendingOptions_i(ryml::NodeRef root, ShaderBlendingOptions& result);
    template bool ReadShaderModulesInfo_i(ryml::NodeRef root, const Ref<ShaderLibrary>& lib, ShaderModulesInfo& result);
    template bool ReadDepthStencilOptions_i(ryml::NodeRef root, ShaderDepthStencilOptions& result);
    template bool ReadAudioClipSettings_i(ryml::NodeRef root, AudioClipSettings& result);

    //endregion
    //region ConstNodeRef

    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, TextureRepeatMode& result);
    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, TextureChannel& result);
    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, TextureFilter& result);
    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, TextureFormat& result);
    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, TextureTiling& result);
    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, BufferMemoryType& result);
    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, TextureResultLayoutType& result);
    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, ShaderCullMode& result);
    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, ShaderFaceDirection& result);
    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, MeshPolygonMode& result);
    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, ColorBlendFactorType& result);
    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, MipMapMode& result);
    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, ColorBlendOp& result);
    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, DepthComparisonFunction& result);
    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, StencilOperation& result);

    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, AudioClipFormat& result);
    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, AudioClipChannelMixMode& result);
    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, AudioClipDitherMode& result);

    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, TextureAnisotropicFilter& result);
    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, uint32_t& result);
    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, float_t& result);
    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, bool& result);

    template bool YamlGetIfArray(c4::yml::ConstNodeRef root, c4::csubstr key, TextureAspectFlags& result);
    template bool YamlGetIfArray(c4::yml::ConstNodeRef root, c4::csubstr key, TextureUsageFlags& result);

    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, TextureDetails& result);
    template bool YamlGetIf(c4::yml::ConstNodeRef root, c4::csubstr key, BlendingOptions& result);

    template bool ReadTextureDetails_i(c4::yml::ConstNodeRef root, TextureDetails& result);
    template bool ReadCubemapTexturePaths_i(c4::yml::ConstNodeRef root, CubemapTexturePaths& result);
    template bool ReadTextureMipMapInfo_i(c4::yml::ConstNodeRef root, TextureMipMapInfo& result);
    template bool ReadBiasOptions_i(c4::yml::ConstNodeRef root, ShaderDepthBiasOptions& result);
    template bool ReadRasterizerOptions_i(c4::yml::ConstNodeRef root, ShaderRasterizerOptions& result);
    template bool ReadBlendingOptions_i(c4::yml::ConstNodeRef root, BlendingOptions& result);
    template bool ReadShaderBlendingOptions_i(c4::yml::ConstNodeRef root, ShaderBlendingOptions& result);
    template bool ReadShaderModulesInfo_i(c4::yml::ConstNodeRef root, const Ref<ShaderLibrary>& lib, ShaderModulesInfo& result);
    template bool ReadDepthStencilOptions_i(c4::yml::ConstNodeRef root, ShaderDepthStencilOptions& result);
    template bool ReadAudioClipSettings_i(c4::yml::ConstNodeRef root, AudioClipSettings& result);

    //endregion
}
