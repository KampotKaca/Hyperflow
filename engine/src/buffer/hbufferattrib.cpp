#include "hyperflow.h"
#include "hinternal.h"
#include "hyaml.h"
#include "hstrconversion.h"

namespace hf::bufferattrib
{
    BufferAttrib Define(const BufferAttribDefinitionInfo& info)
    {
        uint32_t fullStride = 0;
        for (uint32_t i = 0; i < info.formatCount; i++)
        {
            auto& stride = info.pFormats[i];
            stride.lSize = stride.size * BUFFER_DATA_SIZE[(uint32_t)stride.type];
            fullStride += stride.lSize;
        }

        return inter::HF.renderingApi.api.DefineVertBufferAttrib(info, fullStride);
    }

    BufferAttrib Define(const char* assetPath)
    {
        std::string assetLoc = TO_RES_PATH(std::string("bufferattribs/") + assetPath) + ".meta";
        if (!utils::FileExists(assetLoc.c_str()))
        {
            LOG_ERROR("[Hyperflow] Unable to find buffer attrib meta file: %s", assetPath);
            return 0;
        }

        std::vector<char> metadata{};
        if (!utils::ReadFile(assetLoc, true, metadata))
        {
            LOG_ERROR("[Hyperflow] Unable to read buffer attribute meta: %s", assetPath);
            return 0;
        }

        try
        {
            BufferAttribDefinitionInfo info{};

            ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
            ryml::NodeRef root = tree.rootref();
            info.bindingId = std::stoi(root["bindingId"].val().str);

            auto formats = root["formats"];
            std::vector<BufferAttribFormat> formatList{};
            formatList.reserve(formats.num_children());

            for (ryml::NodeRef fmt : formats.children())
            {
                auto type = fmt["type"].val();
                auto size = fmt["size"].val();

                std::string_view typeView{type.str, type.len};

                formatList.emplace_back(STRING_TO_BUFFER_DATA_TYPE(typeView), std::stoi(size.str));
            }

            info.formatCount = (uint32_t)formatList.size();
            info.pFormats = formatList.data();

            auto attrib = Define(info);
            inter::HF.graphicsResources.bufferAttribs[assetPath] = attrib;
            return attrib;
        }catch (...)
        {
            LOG_ERROR("[Hyperflow] Error parsing BufferAttribute: %s", assetPath);
            return 0;
        }
    }

    BufferAttrib GetQuad() { return inter::HF.staticResources.quadAttrib; }
}