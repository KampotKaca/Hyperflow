#include "hyaml.h"
#include "hyperflow.h"
#include "hinternal.h"
#include "hstrconversion.h"

#include <sstream>
#include <fstream>

namespace hf::utils
{
    ivec3 ConvertVersion(const char* version)
    {
        std::istringstream ss(version);
        std::string token;

        ivec3 v{};
        uint32_t id = 0;
        while (std::getline(ss, token, '.') && id < 3)
        {
            v[(int32_t)id] = std::stoi(token);
            id++;
        }

        return v;
    }

    bool ReadFile(const std::string& filename, std::vector<char>& result)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            LOG_ERROR("Unable to open file at location %s", filename.c_str());
            return false;
        }
        const size_t fileSize = file.tellg();
        result = std::vector<char>(fileSize);
        file.seekg(0);
        file.read(result.data(), (int64_t)fileSize);
        file.close();

        return true;
    }

    bool WriteFile(const std::string& filename, const std::vector<char>& data)
    {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open())
        {
            LOG_ERROR("Unable to open file at location %s", filename.c_str());
            return false;
        }
        file.write(data.data(), data.size());
        file.close();
        return true;
    }

    bool FileExists(const char* path)
    {
        struct stat buffer{};
        return stat(path, &buffer) == 0;
    }

    void ReadVertexInputModule(const char* assetPath, ShaderLibraryVertexInputModuleInfo& result)
    {
        const auto assetLoc = TO_RES_PATH(std::string("shadermodules/vertexinputs/") + assetPath) + ".meta";
        std::vector<char> metadata{};
        if(!START_READING(assetLoc.c_str(), metadata)) return;

        try
        {
            ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
            ryml::NodeRef root = tree.rootref();

            {
                const auto v = root["enablePrimitiveRestart"].val();
                const std::string_view vView{v.str, v.len};
                result.enablePrimitiveRestart = ConvertToBool_i(vView);
            }

            {
                const auto v = root["topology"].val();
                const std::string_view vView{v.str, v.len};
                result.topology = STRING_TO_PRIMITIVE_TOPOLOGY_TYPE(vView);
            }

            {
                const auto attributes = root["attributes"];
                result.attributeCount = attributes.num_children();
                uint32_t index = 0;
                for (auto fmt : attributes.children())
                {
                    const auto v = fmt.val();
                    const std::string_view vView{v.str, v.len};
                    result.pAttributes[index] = FindVertexAttribute(std::string(vView).c_str());
                    index++;
                }
            }

        }catch (...)
        {
            LOG_ERROR("[Hyperflow] Error parsing Vertex Input Module: %s", assetLoc.c_str());
        }
    }

    void ReadPreRasterModule(const char* assetPath, ShaderLayout layout, ShaderLibraryPreRasterModuleInfo& result)
    {
        const auto assetLoc = TO_RES_PATH(std::string("shadermodules/prerasters/") + assetPath) + ".meta";
        std::vector<char> metadata{};
        if(!START_READING(assetLoc.c_str(), metadata)) return;

        try
        {
            result.layout = layout;
            ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
            ryml::NodeRef root = tree.rootref();

            {
                const auto v = root["vertexShaderPath"].val();
                const std::string_view vView{v.str, v.len};
                result.vertexShaderPath.path = std::string(vView);
            }

            if (root.has_child("tessellationControlShaderPath"))
            {
                auto node = root["tessellationControlShaderPath"];
                if (node.has_val())
                {
                    const auto v = node.val();
                    const std::string_view vView{v.str, v.len};
                    result.tessellationControlShaderPath = FilePath{ .path = std::string(vView) };
                }
            }

            if (root.has_child("tessellationEvaluationShaderPath"))
            {
                auto node = root["tessellationEvaluationShaderPath"];
                if (node.has_val())
                {
                    const auto v = node.val();
                    const std::string_view vView{v.str, v.len};
                    result.tessellationEvaluationShaderPath = FilePath{ .path = std::string(vView) };
                }
            }

            if (root.has_child("geometryShaderPath"))
            {
                auto node = root["geometryShaderPath"];
                if (node.has_val())
                {
                    const auto v = node.val();
                    const std::string_view vView{v.str, v.len};
                    result.geometryShaderPath = FilePath{ .path = std::string(vView) };
                }
            }

            ReadRasterizerOptions_i(root["options"], result.options);

        }catch (...)
        {
            LOG_ERROR("[Hyperflow] Error parsing Pre Raster Module: %s", assetLoc.c_str());
        }
    }

    void ReadFragmentModule(const char* assetPath, ShaderLayout layout, ShaderLibraryFragmentModuleInfo& result)
    {
        const auto assetLoc = TO_RES_PATH(std::string("shadermodules/fragments/") + assetPath) + ".meta";
        std::vector<char> metadata{};
        if(!START_READING(assetLoc.c_str(), metadata)) return;

        try
        {
            result.layout = layout;
            ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
            ryml::NodeRef root = tree.rootref();

            {
                const auto v = root["fragmentShaderPath"].val();
                const std::string_view vView{v.str, v.len};
                result.fragmentShaderPath.path = std::string(vView);
            }

            ReadDepthStencilOptions_i(root["depthStencilOptions"], result.depthStencilOptions);

        }catch (...)
        {
            LOG_ERROR("[Hyperflow] Error parsing Fragment Module: %s", assetLoc.c_str());
        }
    }

    void ReadFragmentOutputModule(const char* assetPath, ShaderLibraryFragmentOutputModuleInfo& result)
    {
        const auto assetLoc = TO_RES_PATH(std::string("shadermodules/fragmentoutputs/") + assetPath) + ".meta";
        std::vector<char> metadata{};
        if(!START_READING(assetLoc.c_str(), metadata)) return;

        try
        {
            ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
            ryml::NodeRef root = tree.rootref();

            {
                if (root.has_child("blendOp"))
                {
                    auto node = root["blendOp"];
                    if (node.has_val())
                    {
                        const auto v = node.val();
                        const std::string_view vView{v.str, v.len};
                        result.blendOp = STRING_TO_SHADER_BLEND_OPERATION(vView);
                    }
                }

                const auto colorAttachmentsSettings = root["colorAttachmentsSettings"];
                result.colorAttachmentCount = colorAttachmentsSettings.num_children();
                uint32_t index = 0;
                for (auto colorAttachmentSetting : colorAttachmentsSettings.children())
                {
                    ColorAttachmentSettings colorAttachment{};

                    {
                        const auto v = colorAttachmentSetting["colorWriteMask"].val();
                        const std::string_view vView{v.str, v.len};
                        colorAttachment.colorWriteMask = STRING_TO_COLOR_MASKING_FLAGS(vView);
                    }

                    if (colorAttachmentSetting.has_child("blendingOptions"))
                    {
                        ShaderBlendingOptions blendingOptions{};
                        ReadShaderBlendingOptions_i(colorAttachmentSetting["blendingOptions"], blendingOptions);
                        colorAttachment.blendingOptions = blendingOptions;
                    }

                    result.pColorAttachmentsSettings[index] = colorAttachment;
                    index++;
                }
            }
        }catch (...)
        {
            LOG_ERROR("[Hyperflow] Error parsing Fragment Output Module: %s", assetLoc.c_str());
        }
    }
}
