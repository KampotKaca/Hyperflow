#include "htexturepack.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "htexture.h"
#include "hcubemap.h"
#include "hrendertexture.h"
#include "hstrconversion.h"
#include "hyaml.h"

namespace hf
{
    template<typename T>
    static void InitTextures(StaticVector<TexturePack::Binding<T>, MAX_TEXTURES_IN_TEXTURE_PACK>& resultSet,
                             TexturePackBindingInfo<T>* pTextureBindings, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            auto& bInfo = pTextureBindings[i];
            TexturePack::Binding<T> binding
            {
                .sampler = bInfo.sampler,
                .bindingIndex = bInfo.bindingIndex
            };

            for (uint32_t j = 0; j < bInfo.arraySize; j++)
            {
                auto& tex = bInfo.textures[j];
                binding.textures.push_back({
                    .texture = tex.texture,
                    .index = tex.index
                });
            }

            resultSet.push_back(binding);
        }
    }

    template<typename T>
        static void PassTextures(StaticVector<TexturePack::Binding<T>, 8>& src,
            StaticVector<inter::rendering::TexturePackBindInfo_i::TextureInfo, MAX_TEXTURES_IN_TEXTURE_PACK * MAX_TEXTURES_IN_TEXTURE_PACK * 3>& texInfos,
            StaticVector<inter::rendering::TexturePackBindInfo_i, MAX_TEXTURES_IN_TEXTURE_PACK * 3>& texPackBindInfos,
            TexturePackBindingType type)
    {
        for (uint32_t i = 0; i < src.size(); i++)
        {
            auto& binding = src.at(i);
            const uint32_t from = texInfos.size();
            for (uint32_t j = 0; j < binding.textures.size(); j++)
            {
                auto& bi = binding.textures.atC(j);
                if (bi.texture)
                {
                    texInfos.push_back(inter::rendering::TexturePackBindInfo_i::TextureInfo
                    {
                        .texture = bi.texture->handle,
                        .index = bi.index
                    });
                }
                else
                {
                    texInfos.push_back(inter::rendering::TexturePackBindInfo_i::TextureInfo
                    {
                        .texture = nullptr,
                        .index = bi.index
                    });
                }
            }

            texPackBindInfos.push_back(inter::rendering::TexturePackBindInfo_i
            {
                .type = type,
                .sampler = binding.sampler,
                .textures = texInfos.atP(from),
                .arraySize = (uint32_t)(texInfos.size() - from),
                .bindingIndex = binding.bindingIndex
            });
        }
    }

    TexturePack::TexturePack(const TexturePackCreationInfo& info) : layout(info.layout)
    {
        InitTextures(textureBindings, info.pTextureBindings, info.textureBindingCount);
        InitTextures(cubemapBindings, info.pCubemapBindings, info.cubemapBindingCount);
        InitTextures(renderTextureBindings, info.pRenderTextureBindings, info.renderTextureBindingCount);

        StaticVector<inter::rendering::TexturePackBindInfo_i::TextureInfo, MAX_TEXTURES_IN_TEXTURE_PACK * MAX_TEXTURES_IN_TEXTURE_PACK * 3> texInfos{};
        StaticVector<inter::rendering::TexturePackBindInfo_i, MAX_TEXTURES_IN_TEXTURE_PACK * 3> texPackBindInfos{};

        PassTextures(textureBindings, texInfos, texPackBindInfos, TexturePackBindingType::Texture2D);
        PassTextures(cubemapBindings, texInfos, texPackBindInfos, TexturePackBindingType::Cubemap);
        PassTextures(renderTextureBindings, texInfos, texPackBindInfos, TexturePackBindingType::RenderTexture);

        inter::rendering::TexturePackCreationInfo_i tcInfo{};
        tcInfo.layout = layout;
        tcInfo.bindingCount = (uint32_t)texPackBindInfos.size();
        tcInfo.pBindings = texPackBindInfos.atP(0);

        handle = inter::HF.renderingApi.api.CreateTexturePack(tcInfo);
    }

    TexturePack::~TexturePack()
    {
        textureBindings.clear();
        cubemapBindings.clear();
        renderTextureBindings.clear();
        inter::rendering::DestroyTexturePack_i(this);
    }

    bool IsLoaded(const Ref<TexturePack>& texPack) { return texPack->handle; }

    void Destroy(const Ref<TexturePack>& texPack)
    {
        if (inter::rendering::DestroyTexturePack_i(texPack.get()))
            inter::HF.graphicsResources.texturePacks.erase((uint64_t)texPack.get());
    }

    Ref<TexturePack> Create(const TexturePackCreationInfo& info)
    {
        Ref<TexturePack> texPack = MakeRef<TexturePack>(info);
        inter::HF.graphicsResources.texturePacks[(uint64_t)texPack.get()] = texPack;
        return texPack;
    }

    void Destroy(const Ref<TexturePack>* pPacks, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            auto pack = pPacks[i];
            if (inter::rendering::DestroyTexturePack_i(pack.get()))
                inter::HF.graphicsResources.texturePacks.erase((uint64_t)pack.get());
        }
    }

    namespace inter::rendering
    {
        template<typename T>
        static void LoadBindings(c4::yml::NodeRef root, AssetType assetType, std::vector<TexturePackBindingInfo<T>>& ls, std::vector<typename TexturePackBindingInfo<T>::TextureInfo>& infoLs)
        {
            if (root.num_children() > 0)
            {
                ls.reserve(root.num_children());

                for (ryml::NodeRef fmt : root.children())
                {
                    TexturePackBindingInfo<T> texBindingInfo{};
                    texBindingInfo.bindingIndex = std::stoi(fmt["bindingIndex"].val().str);
                    uint32_t textureCount = 0;

                    {
                        const auto v = fmt["sampler"].val();
                        const std::string_view vView{v.str, v.len};
                        texBindingInfo.sampler = FindTextureSampler(vView);
                    }

                    {
                        auto textures = fmt["textures"];
                        textureCount = textures.num_children();
                        if (textures.num_children() > 0)
                        {
                            uint32_t texFrom = infoLs.size();
                            infoLs.reserve(infoLs.size() + textures.num_children());

                            for (ryml::NodeRef texFmt : textures.children())
                            {
                                typename TexturePackBindingInfo<T>::TextureInfo texInfo{};
                                texInfo.index = std::stoi(texFmt["index"].val().str);

                                {
                                    const auto v = texFmt["texture"].val();
                                    const std::string_view vView{v.str, v.len};
                                    texInfo.texture = Cast<T>(GetAsset(vView, assetType));
                                }

                                infoLs.emplace_back(texInfo);
                            }

                            texBindingInfo.textures = &infoLs[texFrom];
                            texBindingInfo.arraySize = textures.num_children();
                        }
                    }

                    if (textureCount > 0) ls.emplace_back(texBindingInfo);
                }
            }
        }

        Ref<TexturePack> CreateTexPackAsset_i(const char* assetPath)
        {
            const auto assetLoc = (TO_RES_PATH_P("texpacks") / assetPath).string() + ".meta";
            const auto moduleFolder = std::filesystem::path(assetLoc).parent_path();

            std::vector<char> metadata{};
            if (!START_READING(assetLoc.c_str(), metadata)) return nullptr;

            try
            {
                TexturePackCreationInfo info{};

                ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
                ryml::NodeRef root = tree.rootref();

                std::vector<TexturePackBindingInfo<Texture>> texList{};
                std::vector<TexturePackBindingInfo<Texture>::TextureInfo> texInfoList{};
                std::vector<TexturePackBindingInfo<Cubemap>> cubList{};
                std::vector<TexturePackBindingInfo<Cubemap>::TextureInfo> cubInfoList{};
                std::vector<TexturePackBindingInfo<RenderTexture>> rtList{};
                std::vector<TexturePackBindingInfo<RenderTexture>::TextureInfo> rtInfoList{};

                {
                    const auto v = root["layout"].val();
                    const std::string_view vView{v.str, v.len};
                    info.layout = FindTextureLayout(vView);
                }

                LoadBindings(root["texBindings"], AssetType::Texture, texList, texInfoList);
                LoadBindings(root["cubBindings"], AssetType::Cubemap, cubList, cubInfoList);
                LoadBindings(root["rtBindings"], AssetType::Texture, rtList, rtInfoList);

                info.pTextureBindings = texList.data();
                info.textureBindingCount = (uint32_t)texList.size();
                info.pCubemapBindings = cubList.data();
                info.cubemapBindingCount = (uint32_t)cubList.size();
                info.pRenderTextureBindings = rtList.data();
                info.renderTextureBindingCount = (uint32_t)rtList.size();

                return MakeRef<TexturePack>(info);
            }
            catch(const HyperException& e)
            {
                log_error(e.GetFile().c_str(), e.GetLine(), e.what());
            }
            catch(const std::exception& e)
            {
                LOG_ERROR("[Hyperflow] Error parsing Texture Pack: %s\nError: %s", assetPath, e.what());
            }
            catch (...)
            {
                LOG_ERROR("[Hyperflow] Unknown error parsing Texture Pack: %s", assetPath);
            }
            return nullptr;
        }

        bool DestroyTexturePack_i(TexturePack* texPack)
        {
            if (texPack->handle)
            {
                std::lock_guard lock(HF.deletedResources.syncLock);
                HF.deletedResources.texturePacks.push_back(texPack->handle);
                texPack->handle = nullptr;
                return true;
            }
            return false;
        }

        void DestroyAllTexturePacks_i(bool internalOnly)
        {
            for (const auto& pack : std::ranges::views::values(HF.graphicsResources.texturePacks))
                DestroyTexturePack_i(pack.get());
            if (!internalOnly) HF.graphicsResources.texturePacks.clear();
        }

        void DestroyAllRenderTextures_i(bool internalOnly)
        {
            for (const auto& tex : std::ranges::views::values(HF.graphicsResources.renderTextures))
                DestroyRenderTexture_i(tex.get());
            if (!internalOnly) HF.graphicsResources.renderTextures.clear();
        }
    }
}
