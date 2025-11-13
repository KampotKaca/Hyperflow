#include "hstrconversion.h"
#include "hyaml.h"

namespace hf
{
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
        for (ryml::NodeRef fmt : node.children())\
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

    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureRepeatMode& result) { YAML_GET(STRING_TO_TEXTURE_REPEAT_MODE); }
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureChannel& result) { YAML_GET(STRING_TO_TEXTURE_CHANNEL); }
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureFilter& result) { YAML_GET(STRING_TO_TEXTURE_FILTER); }
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureFormat& result) { YAML_GET(STRING_TO_TEXTURE_FORMAT); }
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureTiling& result) { YAML_GET(STRING_TO_TEXTURE_TILING); }
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, BufferMemoryType& result) { YAML_GET(STRING_TO_BUFFER_MEMORY_TYPE); }
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureResultLayoutType& result) { YAML_GET(STRING_TO_TEXTURE_RESULT_LAYOUT_TYPE); }
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, ShaderCullMode& result) { YAML_GET(STRING_TO_SHADER_CULL_MODE); }
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, ShaderFaceDirection& result) { YAML_GET(STRING_TO_SHADER_FACE_DIRECTION); }
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, MeshPolygonMode& result) { YAML_GET(STRING_TO_MESH_POLYGON_MODE); }
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, ColorBlendFactorType& result) { YAML_GET(STRING_TO_COLOR_BLEND_FACTOR_TYPE); }
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, MipMapMode& result) { YAML_GET(STRING_TO_MIP_MAP_MODE); }

    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureAnisotropicFilter& result) { YAML_GET_S((TextureAnisotropicFilter)std::stoi); }
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, uint32_t& result) { YAML_GET_S((uint32_t)std::stoi); }
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, float_t& result) { YAML_GET_S(std::stof); }
    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, bool& result) { YAML_GET(ConvertToBool_i); }

    bool YamlGetIfArray(ryml::NodeRef root, c4::csubstr key, TextureAspectFlags& result) { YAML_GET_ARRAY(STRING_TO_TEXTURE_ASPECT_FLAGS); }
    bool YamlGetIfArray(ryml::NodeRef root, c4::csubstr key, TextureUsageFlags& result) { YAML_GET_ARRAY(STRING_TO_TEXTURE_USAGE_FLAGS); }

    bool YamlGetIf(ryml::NodeRef root, c4::csubstr key, TextureDetails& result)
    {
        auto node = root[key];
        return node.readable() && ReadTextureDetails_i(node, result);
    }

#undef YAML_GET
#undef YAML_GET_S
#undef YAML_GET_ARRAY

    bool ReadTextureDetails_i(ryml::NodeRef root, TextureDetails& result)
    {
#define DETAILES_DEF(v)\
        if (!YamlGetIf(root, #v, result.v))\
        {\
            log_warn_s("[Hyperflow] Unable to retrive %s", #v);\
            res = false;\
        }

#define DETAILES_DEF_ARR(v)\
        if (!YamlGetIfArray(root, #v, result.v))\
        {\
            log_warn_s("[Hyperflow] Unable to retrive %s", #v);\
            res = false;\
        }

        bool res = true;
        DETAILES_DEF(format)
        DETAILES_DEF(tiling)
        DETAILES_DEF(memoryType)
        DETAILES_DEF(finalLayout)
        DETAILES_DEF_ARR(aspectFlags)
        DETAILES_DEF_ARR(usageFlags)

#undef DETAILES_DEF
#undef DETAILES_DEF_ARR
        return res;
    }

    bool ReadCubemapTexturePaths_i(ryml::NodeRef root, CubemapTexturePaths& result)
    {
#define CUBE_DEF(n)\
        if (!root.get_if(#n, &result.n))\
        {\
            log_error_s("[Hyperflow] Unable to retrive Cubmap %s texture", #n);\
            res = false;\
        }

        bool res = true;
        CUBE_DEF(left)
        CUBE_DEF(right)
        CUBE_DEF(down)
        CUBE_DEF(up)
        CUBE_DEF(back)
        CUBE_DEF(front)

#undef CUBE_DEF
        return res;
    }

    bool ReadTextureMipMapInfo_i(ryml::NodeRef root, TextureMipMapInfo& result)
    {
#define MIPMAP_DEF(v)\
        if (!YamlGetIf(root, #v, result.v))\
        {\
            res = false;\
            log_warn_s("[Hyperflow] Unable to retrive %s", #v);\
        }

        bool res = true;
        MIPMAP_DEF(mode)
        MIPMAP_DEF(minLod)
        MIPMAP_DEF(maxLod)
        MIPMAP_DEF(lodBias)

#undef MIPMAP_DEF
        return res;
    }

    void ReadBiasOptions_i(ryml::NodeRef root, ShaderDepthBiasOptions& result)
    {
        result.constantFactor = std::stof(root["constantFactor"].val().str);
        result.clamp          = std::stof(root["clamp"].val().str);
        result.slopeFactor    = std::stof(root["slopeFactor"].val().str);
    }

    void ReadRasterizerOptions_i(ryml::NodeRef root, ShaderRasterizerOptions& result)
    {
        {
            const auto v = root["cullMode"].val();
            const std::string_view vView{v.str, v.len};
            result.cullMode = STRING_TO_SHADER_CULL_MODE(vView);
        }

        {
            const auto v = root["faceDirection"].val();
            const std::string_view vView{v.str, v.len};
            result.faceDirection = STRING_TO_SHADER_FACE_DIRECTION(vView);
        }

        {
            const auto v = root["polygonMode"].val();
            const std::string_view vView{v.str, v.len};
            result.polygonMode = STRING_TO_MESH_POLYGON_MODE(vView);
        }

        {
            const auto v = root["enableRasterizerDiscard"].val();
            const std::string_view vView{v.str, v.len};
            result.enableRasterizerDiscard = ConvertToBool_i(vView);
        }

        {
            const auto v = root["enableDepthClamping"].val();
            const std::string_view vView{v.str, v.len};
            result.enableDepthClamping = ConvertToBool_i(vView);
        }

        result.lineWidth = std::stof(root["lineWidth"].val().str);

        if (root.has_child("biasOptions"))
        {
            ShaderDepthBiasOptions biasOptions{};
            ReadBiasOptions_i(root["biasOptions"], biasOptions);
            result.biasOptions = biasOptions;
        }
    }

    void ReadBlendingOptions_i(ryml::NodeRef root, BlendingOptions& result)
    {
        {
            const auto v = root["srcFactor"].val();
            const std::string_view vView{v.str, v.len};
            result.srcFactor = STRING_TO_COLOR_BLEND_FACTOR_TYPE(vView);
        }

        {
            const auto v = root["dstFactor"].val();
            const std::string_view vView{v.str, v.len};
            result.dstFactor = STRING_TO_COLOR_BLEND_FACTOR_TYPE(vView);
        }

        {
            const auto v = root["op"].val();
            const std::string_view vView{v.str, v.len};
            result.op = STRING_TO_COLOR_BLEND_OPERATION(vView);
        }
    }

    void ReadBlendingOptions_i(c4::yml::ConstNodeRef root, BlendingOptions& result)
    {
        {
            const auto v = root["srcFactor"].val();
            const std::string_view vView{v.str, v.len};
            result.srcFactor = STRING_TO_COLOR_BLEND_FACTOR_TYPE(vView);
        }

        {
            const auto v = root["dstFactor"].val();
            const std::string_view vView{v.str, v.len};
            result.dstFactor = STRING_TO_COLOR_BLEND_FACTOR_TYPE(vView);
        }

        {
            const auto v = root["op"].val();
            const std::string_view vView{v.str, v.len};
            result.op = STRING_TO_COLOR_BLEND_OPERATION(vView);
        }
    }

    void ReadShaderBlendingOptions_i(ryml::NodeRef root, ShaderBlendingOptions& result)
    {
        if (root.has_child("color")) ReadBlendingOptions_i(root["color"], result.color);
        else log_error("[Hyperflow] %s", "Unable to find color of the blending options.");

        if (root.has_child("alpha")) ReadBlendingOptions_i(root["alpha"], result.alpha);
        else log_error("[Hyperflow] %s", "Unable to find alpha of the blending options.");
    }

    void ReadShaderBlendingOptions_i(c4::yml::ConstNodeRef root, ShaderBlendingOptions& result)
    {
        if (root.has_child("color")) ReadBlendingOptions_i(root["color"], result.color);
        else log_error("[Hyperflow] %s", "Unable to find color of the blending options.");

        if (root.has_child("alpha")) ReadBlendingOptions_i(root["alpha"], result.alpha);
        else log_error("[Hyperflow] %s", "Unable to find alpha of the blending options.");
    }

    void ReadShaderModulesInfo_i(c4::yml::ConstNodeRef root, const Ref<ShaderLibrary>& lib, ShaderModulesInfo& result)
    {
        {
            const auto v = root["vertexInputModule"].val();
            const std::string_view vView{v.str, v.len};
            result.vertexInputModuleId = GetVertexInputModule(lib, vView);
        }

        {
            const auto v = root["preRasterModule"].val();
            const std::string_view vView{v.str, v.len};
            result.preRasterModuleId = GetPreRasterModule(lib, vView);
        }

        {
            const auto v = root["fragmentModule"].val();
            const std::string_view vView{v.str, v.len};
            result.fragmentModuleId = GetFragmentModule(lib, vView);
        }

        {
            const auto v = root["fragmentOutputModule"].val();
            const std::string_view vView{v.str, v.len};
            result.fragmentOutputModuleId = GetFragmentOutputModule(lib, vView);
        }
    }

    void ReadDepthStencilOptions_i(ryml::NodeRef root, ShaderDepthStencilOptions& result)
    {
        {
            const auto v = root["enableDepth"].val();
            const std::string_view vView{v.str, v.len};
            result.enableDepth = ConvertToBool_i(vView);
        }

        {
            const auto v = root["writeDepth"].val();
            const std::string_view vView{v.str, v.len};
            result.writeDepth = ConvertToBool_i(vView);
        }

        {
            const auto v = root["comparisonFunc"].val();
            const std::string_view vView{v.str, v.len};
            result.comparisonFunc = STRING_TO_DEPTH_COMPARISON_FUNCTION(vView);
        }

        {
            const auto v = root["enableDepthBounds"].val();
            const std::string_view vView{v.str, v.len};
            result.enableDepthBounds = ConvertToBool_i(vView);
        }

        {
            const auto v = root["enableStencil"].val();
            const std::string_view vView{v.str, v.len};
            result.enableStencil = ConvertToBool_i(vView);
        }

        {
            const auto v = root["frontStencil"].val();
            const std::string_view vView{v.str, v.len};
            result.frontStencil = STRING_TO_STENCIL_OPERATION(vView);
        }

        {
            const auto v = root["backStencil"].val();
            const std::string_view vView{v.str, v.len};
            result.backStencil = STRING_TO_STENCIL_OPERATION(vView);
        }

        result.depthBounds.x = std::stof(root["minDepthBounds"].val().str);
        result.depthBounds.y = std::stof(root["maxDepthBounds"].val().str);
    }

    void ReadAudioClipSettings_i(c4::yml::ConstNodeRef root, AudioClipSettings& result)
    {
        result.sampleRate = (uint32_t)std::stoi(root["sampleRate"].val().str);
        {
            const auto v = root["format"].val();
            const std::string_view vView{v.str, v.len};
            result.format = STRING_TO_AUDIO_CLIP_FORMAT(vView);
        }

        {
            const auto v = root["channelMixMode"].val();
            const std::string_view vView{v.str, v.len};
            result.channelMixMode = STRING_TO_AUDIO_CLIP_CHANNEL_MIX_MODE(vView);
        }

        {
            const auto v = root["ditherMode"].val();
            const std::string_view vView{v.str, v.len};
            result.ditherMode = STRING_TO_AUDIO_CLIP_DITHER_MODE(vView);
        }
    }
}
