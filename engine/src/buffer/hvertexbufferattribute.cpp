#include "hyperflow.h"
#include "hinternal.h"
#include "hyaml.h"
#include "hstrconversion.h"

namespace hf
{
    VertexBufferAttribute Define(const VertexBufferAttributeDefinitionInfo& info)
    {
        uint32_t fullStride = 0;
        for (uint32_t i = 0; i < info.formatCount; i++)
        {
            auto& stride = info.pFormats[i];
            stride.lSize = stride.size * BUFFER_DATA_SIZE[(uint32_t)stride.type];
            fullStride += stride.lSize;
        }

        return ir::HF.renderingApi.api.DefineVertexBufferAttribute(info, fullStride);
    }

    VertexBufferAttribute DefineVertexAttributeAsset(const char* assetPath)
    {
        const auto assetLoc = TO_RES_PATH(std::string("vertexattributes/") + assetPath) + ".meta";
        List<char> metadata{};
        if (!START_READING(assetLoc.c_str(), metadata)) return 0;

        try
        {
            VertexBufferAttributeDefinitionInfo info{};

            ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
            ryml::NodeRef root = tree.rootref();
            info.bindingId = std::stoi(root["bindingId"].val().str);
            info.locationOffset = std::stoi(root["locationOffset"].val().str);

            {
                const auto v = root["inputRate"].val();
                const std::string_view vView{v.str, v.len};
                info.inputRate = STRING_TO_BUFFER_INPUT_RATE(vView);
            }

            auto formats = root["formats"];
            List<BufferAttribFormat> formatList{};
            formatList.reserve(formats.num_children());

            for (ryml::NodeRef fmt : formats.children())
            {
                const auto type = fmt["type"].val();
                const auto size = fmt["size"].val();

                const std::string_view typeView{type.str, type.len};

                formatList.emplace_back(STRING_TO_BUFFER_DATA_TYPE(typeView), std::stoi(size.str));
            }

            info.formatCount = (uint32_t)formatList.size();
            info.pFormats = formatList.data();

            const auto attrib = Define(info);
            ir::HF.graphicsResources.vertexAttributes[assetPath] = attrib;
            return attrib;
        }catch (...)
        {
            log_error("[Hyperflow] Error parsing BufferAttribute: %s", assetPath);
            return 0;
        }
    }

    uint32_t GetVertexSize(VertexBufferAttribute attribute) { return ir::HF.renderingApi.api.GetVertexBufferAttributeSize(attribute); }
}