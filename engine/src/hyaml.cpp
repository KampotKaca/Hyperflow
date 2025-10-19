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

    void ReadTextureDetails_i(ryml::NodeRef root, TextureDetails& result)
    {
        {
            const auto v = root["format"].val();
            const std::string_view vView{v.str, v.len};
            result.format = STRING_TO_TEXTURE_FORMAT(vView);
        }

        {
            auto aspectFlags = root["aspectFlags"];
            for (ryml::NodeRef fmt : aspectFlags.children())
            {
                const auto v = fmt.val();
                const std::string_view vView{v.str, v.len};
                result.aspectFlags |= STRING_TO_TEXTURE_ASPECT_FLAGS(vView);
            }
        }

        {
            const auto v = root["tiling"].val();
            const std::string_view vView{v.str, v.len};
            result.tiling = STRING_TO_TEXTURE_TILING(vView);
        }

        {
            auto usageFlags = root["usageFlags"];
            for (ryml::NodeRef fmt : usageFlags.children())
            {
                const auto v = fmt.val();
                const std::string_view vView{v.str, v.len};
                result.usageFlags |= STRING_TO_TEXTURE_USAGE_FLAGS(vView);
            }
        }

        {
            const auto v = root["memoryType"].val();
            const std::string_view vView{v.str, v.len};
            result.memoryType = STRING_TO_BUFFER_MEMORY_TYPE(vView);
        }

        {
            const auto v = root["finalLayout"].val();
            const std::string_view vView{v.str, v.len};
            result.finalLayout = STRING_TO_TEXTURE_RESULT_LAYOUT_TYPE(vView);
        }
    }

    void ReadCubemapTexturePaths_i(ryml::NodeRef root, CubemapTexturePaths& result)
    {
        {
            const auto v = root["left"].val();
            const std::string_view vView{v.str, v.len};
            result.left.path = vView;
        }

        {
            const auto v = root["right"].val();
            const std::string_view vView{v.str, v.len};
            result.right.path = vView;
        }

        {
            const auto v = root["down"].val();
            const std::string_view vView{v.str, v.len};
            result.down.path = vView;
        }

        {
            const auto v = root["up"].val();
            const std::string_view vView{v.str, v.len};
            result.up.path = vView;
        }

        {
            const auto v = root["back"].val();
            const std::string_view vView{v.str, v.len};
            result.back.path = vView;
        }

        {
            const auto v = root["front"].val();
            const std::string_view vView{v.str, v.len};
            result.front.path = vView;
        }
    }

    void ReadTextureMipMapInfo_i(ryml::NodeRef root, TextureMipMapInfo& result)
    {
        {
            const auto v = root["mode"].val();
            const std::string_view vView{v.str, v.len};
            result.mode = STRING_TO_MIP_MAP_MODE(vView);
        }

        result.minLod  = std::stof(root["minLod"].val().str);
        result.maxLod  = std::stof(root["maxLod"].val().str);
        result.lodBias = std::stof(root["lodBias"].val().str);
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
        else LOG_ERROR("[Hyperflow] %s", "Unable to find color of the blending options.");

        if (root.has_child("alpha")) ReadBlendingOptions_i(root["alpha"], result.alpha);
        else LOG_ERROR("[Hyperflow] %s", "Unable to find alpha of the blending options.");
    }

    void ReadShaderBlendingOptions_i(c4::yml::ConstNodeRef root, ShaderBlendingOptions& result)
    {
        if (root.has_child("color")) ReadBlendingOptions_i(root["color"], result.color);
        else LOG_ERROR("[Hyperflow] %s", "Unable to find color of the blending options.");

        if (root.has_child("alpha")) ReadBlendingOptions_i(root["alpha"], result.alpha);
        else LOG_ERROR("[Hyperflow] %s", "Unable to find alpha of the blending options.");
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
            std::string_view vView{v.str, v.len};
            result.format = STRING_TO_AUDIO_CLIP_FORMAT(vView);
        }

        {
            const auto v = root["channelMixMode"].val();
            std::string_view vView{v.str, v.len};
            result.channelMixMode = STRING_TO_AUDIO_CLIP_CHANNEL_MIX_MODE(vView);
        }

        {
            const auto v = root["ditherMode"].val();
            std::string_view vView{v.str, v.len};
            result.ditherMode = STRING_TO_AUDIO_CLIP_DITHER_MODE(vView);
        }
    }
}
