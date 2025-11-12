#include "hyperflow.h"
#include "hyaml.h"
#include "hstrconversion.h"
#include "hinternal.h"

namespace hf
{
    TextureLayout Define(const TextureLayoutDefinitionInfo& info) { return ir::HF.renderingApi.api.DefineTextureLayout(info); }

    TextureLayout DefineTextureLayoutAsset(const char* assetPath)
    {
        const auto assetLoc = TO_RES_PATH(std::string("texturelayouts/") + assetPath) + ".meta";
        List<char> metadata{};
        START_READING(assetLoc.c_str(), metadata);

        TextureLayoutDefinitionInfo info{};
        List<TextureLayoutBindingInfo> texLayoutBindings{};

        ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
        ryml::NodeRef root = tree.rootref();

        {
            auto bindings = root["bindings"];
            texLayoutBindings.reserve(bindings.num_children());
            for (ryml::NodeRef childBinding : bindings.children())
            {
                TextureLayoutBindingInfo bindingInfo{};
                {
                    bindingInfo.bindingId = (uint32_t)std::stoi(childBinding["bindingId"].val().str);
                    bindingInfo.arraySize = (uint32_t)std::stoi(childBinding["arraySize"].val().str);

                    auto usageFlags = childBinding["usageFlags"];
                    for (ryml::NodeRef fmt : usageFlags.children())
                    {
                        const auto v = fmt.val();
                        const std::string_view vView{v.str, v.len};
                        bindingInfo.usageFlags |= STRING_TO_SHADER_USAGE_FLAGS(vView);
                    }
                }
                texLayoutBindings.push_back(bindingInfo);
            }
        }

        info.pBindings = texLayoutBindings.data();
        info.bindingCount = (uint32_t)texLayoutBindings.size();

        const auto layout = Define(info);
        ir::HF.graphicsResources.textureLayouts[assetPath] = layout;
        return layout;
    }
}