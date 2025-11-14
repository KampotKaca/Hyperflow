#include "hstrconversion.h"
#include "hyaml.h"

namespace hf
{
    //region Defines
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

#define YAML_GET_FLAGS(func)\
    auto node = root[key];\
    result = {};\
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

#define YAML_GET_ARRAY_I(func)\
    auto node = root[key];\
    int32_t c = -1;\
    if (node.readable() && node.num_children() > 0)\
    {\
        uint32_t id = 0;\
        c = node.num_children();\
        for (auto fmt : node.children())\
        {\
            if (fmt.readable())\
            {\
                const auto v = fmt.val();\
                const std::string_view vView{v.str, v.len};\
                resultArr[id] = func(vView);\
            }\
            else return -1;\
            id++;\
        }\
        return c;\
    }\
    else return c

#define DEF(v)\
    if (!YamlGetIf_i(root, #v, result.v))\
    {\
        log_warn_s("[Hyperflow] Unable to retrieve %s", #v);\
        res = false;\
    }

#define DEF_FLAGS(v)\
    if (!YamlGetIfFlags_i(root, #v, result.v))\
    {\
        log_warn_s("[Hyperflow] Unable to retrieve %s", #v);\
        res = false;\
    }

#define DEF_ARR(v)\
    if (!YamlGetIfArray_i(root, #v, result.v))\
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
    //endregion
    //region Implementations

    bool ConvertToBool_i(const std::string_view& view)
    {
        std::string lowerStr(view);
        std::ranges::transform(lowerStr, lowerStr.begin(), tolower);

        return lowerStr == "true" || lowerStr == "1" || lowerStr == "yes";
    }

    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, TextureRepeatMode& result) { YAML_GET(STRING_TO_TEXTURE_REPEAT_MODE); }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, TextureChannel& result) { YAML_GET(STRING_TO_TEXTURE_CHANNEL); }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, TextureFilter& result) { YAML_GET(STRING_TO_TEXTURE_FILTER); }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, TextureFormat& result) { YAML_GET(STRING_TO_TEXTURE_FORMAT); }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, TextureTiling& result) { YAML_GET(STRING_TO_TEXTURE_TILING); }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, BufferMemoryType& result) { YAML_GET(STRING_TO_BUFFER_MEMORY_TYPE); }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, TextureResultLayoutType& result) { YAML_GET(STRING_TO_TEXTURE_RESULT_LAYOUT_TYPE); }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, ShaderCullMode& result) { YAML_GET(STRING_TO_SHADER_CULL_MODE); }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, ShaderFaceDirection& result) { YAML_GET(STRING_TO_SHADER_FACE_DIRECTION); }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, MeshPolygonMode& result) { YAML_GET(STRING_TO_MESH_POLYGON_MODE); }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, ColorBlendFactorType& result) { YAML_GET(STRING_TO_COLOR_BLEND_FACTOR_TYPE); }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, MipMapMode& result) { YAML_GET(STRING_TO_MIP_MAP_MODE); }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, ColorBlendOp& result) { YAML_GET(STRING_TO_COLOR_BLEND_OPERATION); }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, DepthComparisonFunction& result) { YAML_GET(STRING_TO_DEPTH_COMPARISON_FUNCTION); }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, StencilOperation& result) { YAML_GET(STRING_TO_STENCIL_OPERATION); }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, ColorMaskingFlags& result) { YAML_GET(STRING_TO_COLOR_MASKING_FLAGS); }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, ShaderBlendOp& result) { YAML_GET(STRING_TO_SHADER_BLEND_OPERATION); }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, MeshPrimitiveTopologyType& result) { YAML_GET(STRING_TO_MESH_PRIMITIVE_TOPOLOGY_TYPE); }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, ComparisonOperation& result) { YAML_GET(STRING_TO_COMPARISON_OPERATION); }

    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, AudioClipFormat& result) { YAML_GET(STRING_TO_AUDIO_CLIP_FORMAT); }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, AudioClipChannelMixMode& result) { YAML_GET(STRING_TO_AUDIO_CLIP_CHANNEL_MIX_MODE); }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, AudioClipDitherMode& result) { YAML_GET(STRING_TO_AUDIO_CLIP_DITHER_MODE); }

    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, TextureAnisotropicFilter& result) { YAML_GET_S((TextureAnisotropicFilter)std::stoi); }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, uint32_t& result) { YAML_GET_S((uint32_t)std::stoi); }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, float_t& result) { YAML_GET_S(std::stof); }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, bool& result) { YAML_GET(ConvertToBool_i); }

    template<typename T> bool YamlGetIfFlags_i(T root, c4::csubstr key, TextureAspectFlags& result) { YAML_GET_FLAGS(STRING_TO_TEXTURE_ASPECT_FLAGS); }
    template<typename T> bool YamlGetIfFlags_i(T root, c4::csubstr key, TextureUsageFlags& result) { YAML_GET_FLAGS(STRING_TO_TEXTURE_USAGE_FLAGS); }
    template<typename T> bool YamlGetIfFlags_i(T root, c4::csubstr key, MeshDataType& result) { YAML_GET_FLAGS(STRING_TO_MESH_DATA_TYPE); }
    template<typename T> bool YamlGetIfFlags_i(T root, c4::csubstr key, ShaderUsageStageFlags& result) { YAML_GET_FLAGS(STRING_TO_SHADER_USAGE_FLAGS); }

    template<typename T> int32_t YamlGetIfArray_i(T root, c4::csubstr key, VertexBufferAttribute* resultArr) { YAML_GET_ARRAY_I(FindVertexAttribute); }
    template<typename T> int32_t YamlGetIfArray_i(T root, c4::csubstr key, ColorAttachmentSettings* resultArr)
    {
        auto colorAttachmentsSettings = root[key];
        int32_t c = 0;
        if (colorAttachmentsSettings.readable() && colorAttachmentsSettings.num_children() > 0)
        {
            uint32_t index = 0;
            c = colorAttachmentsSettings.num_children();
            for (auto colorAttachmentSetting : colorAttachmentsSettings.children())
            {
                ColorAttachmentSettings colorAttachment{};

                if (!YamlGetIf_i(colorAttachmentSetting, "colorWriteMask", colorAttachment.colorWriteMask))
                {
                    log_warn_s("[Hyperflow] Unable to retrieve colorWriteMask %i", index);
                    return -1;
                }

                {
                    ShaderBlendingOptions blendingOptions{};
                    if (YamlGetIf_i(colorAttachmentSetting, "blendingOptions", blendingOptions)) colorAttachment.blendingOptions = blendingOptions;
                }

                resultArr[index] = colorAttachment;
                index++;
            }
            return c;
        }
        return c;
    }

    template<typename T> bool ReadTextureDetails_i(T root, TextureDetails& result)
    {
        bool res = true;
        DEF(format)
        DEF(tiling)
        DEF(memoryType)
        DEF(finalLayout)
        DEF_FLAGS(aspectFlags)
        DEF_FLAGS(usageFlags)

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
            ShaderDepthBiasOptions biasOptions{};
            if (YamlGetIf_i(root, "biasOptions", biasOptions)) result.biasOptions = biasOptions;
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
    template<typename T> bool ReadMeshStats_i(T root, MeshStats& result)
    {
        bool res = true;
        DEF_FLAGS(typeFlags)
        DEF(memoryType)

        if (!YamlGetIf_VertexAttribute_i(root, "vertexAttribute", result.vertexAttribute))
        {
            log_warn_s("[Hyperflow] Unable to retrieve vertexAttribute");
            res = false;
        }
        return res;
    }

    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, TextureDetails& result)
    {
        auto node = root[key];
        return node.readable() && ReadTextureDetails_i(node, result);
    }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, CubemapTexturePaths& result)
    {
        auto node = root[key];
        return node.readable() && ReadCubemapTexturePaths_i(node, result);
    }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, TextureMipMapInfo& result)
    {
        auto node = root[key];
        return node.readable() && ReadTextureMipMapInfo_i(node, result);
    }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, ShaderDepthBiasOptions& result)
    {
        auto node = root[key];
        return node.readable() && ReadBiasOptions_i(node, result);
    }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, ShaderRasterizerOptions& result)
    {
        auto node = root[key];
        return node.readable() && ReadRasterizerOptions_i(node, result);
    }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, BlendingOptions& result)
    {
        auto node = root[key];
        return node.readable() && ReadBlendingOptions_i(node, result);
    }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, ShaderBlendingOptions& result)
    {
        auto node = root[key];
        return node.readable() && ReadShaderBlendingOptions_i(node, result);
    }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, MeshStats& result)
    {
        auto node = root[key];
        return node.readable() && ReadMeshStats_i(node, result);
    }

    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, const Ref<ShaderLibrary>& lib, ShaderModulesInfo& result)
    {
        auto node = root[key];
        return node.readable() && ReadShaderModulesInfo_i(node, lib, result);
    }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, ShaderDepthStencilOptions& result)
    {
        auto node = root[key];
        return node.readable() && ReadDepthStencilOptions_i(node, result);
    }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, AudioClipSettings& result)
    {
        auto node = root[key];
        return node.readable() && ReadAudioClipSettings_i(node, result);
    }
    template<typename T> bool YamlGetIf_i(T root, c4::csubstr key, Ref<ShaderLibrary>& result)
    {
        auto node = root[key];
        if (node.readable())
        {
            const auto v = node.val();
            const std::string_view vView{v.str, v.len};
            result = Cast<ShaderLibrary>(GetAsset(vView, AssetType::ShaderLibrary));
            return true;
        }
        return false;
    }

    template<typename T> bool YamlGetIf_ShaderLayout_i(T root, c4::csubstr key, ShaderLayout& result) { YAML_GET(FindShaderLayout); }
    template<typename T> bool YamlGetIf_VertexAttribute_i(T root, c4::csubstr key, VertexBufferAttribute& result) { YAML_GET(FindVertexAttribute); }

    //endregion
    //region Undefines
#undef YAML_GET
#undef YAML_GET_S
#undef YAML_GET_FLAGS

#undef DEF
#undef DEF_ARR
#undef DEF_GET
    //endregion
    //region NodeRef

    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, TextureRepeatMode& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, TextureChannel& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, TextureFilter& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, TextureFormat& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, TextureTiling& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, BufferMemoryType& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, TextureResultLayoutType& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, ShaderCullMode& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, ShaderFaceDirection& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, MeshPolygonMode& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, ColorBlendFactorType& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, MipMapMode& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, ColorBlendOp& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, DepthComparisonFunction& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, StencilOperation& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, ColorMaskingFlags& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, ShaderBlendOp& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, MeshPrimitiveTopologyType& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, ComparisonOperation& result);

    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, AudioClipFormat& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, AudioClipChannelMixMode& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, AudioClipDitherMode& result);

    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, TextureAnisotropicFilter& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, uint32_t& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, float_t& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, bool& result);

    template bool YamlGetIfFlags_i(ryml::NodeRef root, c4::csubstr key, TextureAspectFlags& result);
    template bool YamlGetIfFlags_i(ryml::NodeRef root, c4::csubstr key, TextureUsageFlags& result);
    template bool YamlGetIfFlags_i(ryml::NodeRef root, c4::csubstr key, MeshDataType& result);
    template bool YamlGetIfFlags_i(ryml::NodeRef root, c4::csubstr key, ShaderUsageStageFlags& result);

    template int32_t YamlGetIfArray_i(ryml::NodeRef root, c4::csubstr key, VertexBufferAttribute* resultArr);
    template int32_t YamlGetIfArray_i(ryml::NodeRef root, c4::csubstr key, ColorAttachmentSettings* resultArr);

    template bool ReadTextureDetails_i(ryml::NodeRef root, TextureDetails& result);
    template bool ReadCubemapTexturePaths_i(ryml::NodeRef root, CubemapTexturePaths& result);
    template bool ReadTextureMipMapInfo_i(ryml::NodeRef root, TextureMipMapInfo& result);
    template bool ReadBiasOptions_i(ryml::NodeRef root, ShaderDepthBiasOptions& result);
    template bool ReadRasterizerOptions_i(ryml::NodeRef root, ShaderRasterizerOptions& result);
    template bool ReadBlendingOptions_i(ryml::NodeRef root, BlendingOptions& result);
    template bool ReadShaderBlendingOptions_i(ryml::NodeRef root, ShaderBlendingOptions& result);
    template bool ReadMeshStats_i(ryml::NodeRef root, MeshStats& result);

    template bool ReadShaderModulesInfo_i(ryml::NodeRef root, const Ref<ShaderLibrary>& lib, ShaderModulesInfo& result);
    template bool ReadDepthStencilOptions_i(ryml::NodeRef root, ShaderDepthStencilOptions& result);
    template bool ReadAudioClipSettings_i(ryml::NodeRef root, AudioClipSettings& result);

    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, TextureDetails& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, CubemapTexturePaths& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, TextureMipMapInfo& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, ShaderDepthBiasOptions& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, ShaderRasterizerOptions& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, BlendingOptions& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, ShaderBlendingOptions& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, MeshStats& result);

    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, const Ref<ShaderLibrary>& lib, ShaderModulesInfo& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, ShaderDepthStencilOptions& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, AudioClipSettings& result);
    template bool YamlGetIf_i(ryml::NodeRef root, c4::csubstr key, Ref<ShaderLibrary>& result);

    template bool YamlGetIf_ShaderLayout_i(ryml::NodeRef root, c4::csubstr key, ShaderLayout& result);
    template bool YamlGetIf_VertexAttribute_i(ryml::NodeRef root, c4::csubstr key, VertexBufferAttribute& result);

    //endregion
    //region ConstNodeRef

    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, TextureRepeatMode& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, TextureChannel& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, TextureFilter& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, TextureFormat& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, TextureTiling& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, BufferMemoryType& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, TextureResultLayoutType& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, ShaderCullMode& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, ShaderFaceDirection& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, MeshPolygonMode& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, ColorBlendFactorType& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, MipMapMode& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, ColorBlendOp& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, DepthComparisonFunction& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, StencilOperation& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, ColorMaskingFlags& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, ShaderBlendOp& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, MeshPrimitiveTopologyType& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, ComparisonOperation& result);

    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, AudioClipFormat& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, AudioClipChannelMixMode& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, AudioClipDitherMode& result);

    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, TextureAnisotropicFilter& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, uint32_t& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, float_t& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, bool& result);

    template bool YamlGetIfFlags_i(c4::yml::ConstNodeRef root, c4::csubstr key, TextureAspectFlags& result);
    template bool YamlGetIfFlags_i(c4::yml::ConstNodeRef root, c4::csubstr key, TextureUsageFlags& result);
    template bool YamlGetIfFlags_i(c4::yml::ConstNodeRef root, c4::csubstr key, MeshDataType& result);
    template bool YamlGetIfFlags_i(c4::yml::ConstNodeRef root, c4::csubstr key, ShaderUsageStageFlags& result);

    template int32_t YamlGetIfArray_i(c4::yml::ConstNodeRef root, c4::csubstr key, VertexBufferAttribute* resultArr);
    template int32_t YamlGetIfArray_i(c4::yml::ConstNodeRef root, c4::csubstr key, ColorAttachmentSettings* resultArr);

    template bool ReadTextureDetails_i(c4::yml::ConstNodeRef root, TextureDetails& result);
    template bool ReadCubemapTexturePaths_i(c4::yml::ConstNodeRef root, CubemapTexturePaths& result);
    template bool ReadTextureMipMapInfo_i(c4::yml::ConstNodeRef root, TextureMipMapInfo& result);
    template bool ReadBiasOptions_i(c4::yml::ConstNodeRef root, ShaderDepthBiasOptions& result);
    template bool ReadRasterizerOptions_i(c4::yml::ConstNodeRef root, ShaderRasterizerOptions& result);
    template bool ReadBlendingOptions_i(c4::yml::ConstNodeRef root, BlendingOptions& result);
    template bool ReadShaderBlendingOptions_i(c4::yml::ConstNodeRef root, ShaderBlendingOptions& result);
    template bool ReadMeshStats_i(c4::yml::ConstNodeRef root, MeshStats& result);

    template bool ReadShaderModulesInfo_i(c4::yml::ConstNodeRef root, const Ref<ShaderLibrary>& lib, ShaderModulesInfo& result);
    template bool ReadDepthStencilOptions_i(c4::yml::ConstNodeRef root, ShaderDepthStencilOptions& result);
    template bool ReadAudioClipSettings_i(c4::yml::ConstNodeRef root, AudioClipSettings& result);

    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, TextureDetails& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, CubemapTexturePaths& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, TextureMipMapInfo& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, ShaderDepthBiasOptions& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, ShaderRasterizerOptions& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, BlendingOptions& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, ShaderBlendingOptions& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, MeshStats& result);

    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, const Ref<ShaderLibrary>& lib, ShaderModulesInfo& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, ShaderDepthStencilOptions& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, AudioClipSettings& result);
    template bool YamlGetIf_i(c4::yml::ConstNodeRef root, c4::csubstr key, Ref<ShaderLibrary>& result);

    template bool YamlGetIf_ShaderLayout_i(c4::yml::ConstNodeRef root, c4::csubstr key, ShaderLayout& result);
    template bool YamlGetIf_VertexAttribute_i(c4::yml::ConstNodeRef root, c4::csubstr key, VertexBufferAttribute& result);

    //endregion
}
