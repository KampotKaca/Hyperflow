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
            hassert(bindings.readable() && bindings.num_children() > 0, "[Hyperflow] Texture layout %s has invalid bindings!", assetPath)

            texLayoutBindings.reserve(bindings.num_children());
            for (ryml::NodeRef childBinding : bindings.children())
            {
                TextureLayoutBindingInfo bindingInfo{};
                {
                    hassert(YamlGetIf_i(childBinding, "bindingId", bindingInfo.bindingId), "[Hyperflow] Texture layout %s has invalid bindingId!", assetPath)
                    hassert(YamlGetIf_i(childBinding, "arraySize", bindingInfo.arraySize), "[Hyperflow] Texture layout %s has invalid arraySize!", assetPath)
                    hassert(YamlGetIfFlags_i(childBinding, "usageFlags", bindingInfo.usageFlags), "[Hyperflow] Texture layout %s has invalid usageFlags!", assetPath)
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