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
    static void InitTextures(SmallList<TexturePack::Binding<T>, MAX_TEXTURES_IN_TEXTURE_PACK>& resultSet,
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
        static void PassTextures(SmallList<TexturePack::Binding<T>, MAX_TEXTURES_IN_TEXTURE_PACK>& src,
            SmallList<ir::rdr::TexturePackBindInfo_i::TextureInfo, MAX_TEXTURES_IN_TEXTURE_PACK * MAX_TEXTURES_IN_TEXTURE_PACK * 3>& texInfos,
            SmallList<ir::rdr::TexturePackBindInfo_i, MAX_TEXTURES_IN_TEXTURE_PACK * 3>& texPackBindInfos,
            TexturePackBindingType type)
    {
        for (uint32_t i = 0; i < src.size(); i++)
        {
            auto& binding = src.at(i);
            const uint32_t from = texInfos.size();
            for (uint32_t j = 0; j < binding.textures.size(); j++)
            {
                auto& bi = binding.textures[j];
                if (bi.texture)
                {
                    texInfos.push_back(ir::rdr::TexturePackBindInfo_i::TextureInfo
                    {
                        .texture = bi.texture->handle,
                        .index = bi.index
                    });
                }
                else
                {
                    texInfos.push_back(ir::rdr::TexturePackBindInfo_i::TextureInfo
                    {
                        .texture = nullptr,
                        .index = bi.index
                    });
                }
            }

            texPackBindInfos.push_back(ir::rdr::TexturePackBindInfo_i
            {
                .type = type,
                .sampler = binding.sampler,
                .textures = &texInfos[from],
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

        SmallList<ir::rdr::TexturePackBindInfo_i::TextureInfo, MAX_TEXTURES_IN_TEXTURE_PACK * MAX_TEXTURES_IN_TEXTURE_PACK * 3> texInfos{};
        SmallList<ir::rdr::TexturePackBindInfo_i, MAX_TEXTURES_IN_TEXTURE_PACK * 3> texPackBindInfos{};

        PassTextures(textureBindings, texInfos, texPackBindInfos, TexturePackBindingType::Texture2D);
        PassTextures(cubemapBindings, texInfos, texPackBindInfos, TexturePackBindingType::Cubemap);
        PassTextures(renderTextureBindings, texInfos, texPackBindInfos, TexturePackBindingType::RenderTexture);

        ir::rdr::TexturePackCreationInfo_i tcInfo{};
        tcInfo.layout = layout;
        tcInfo.bindingCount = (uint32_t)texPackBindInfos.size();
        tcInfo.pBindings = &texPackBindInfos[0];

        handle = ir::HF.renderingApi.api.CreateTexturePack(tcInfo);
    }

    TexturePack::~TexturePack()
    {
        textureBindings.clear();
        cubemapBindings.clear();
        renderTextureBindings.clear();
        ir::rdr::DestroyTexturePack_i(this);
    }

    bool IsLoaded(const Ref<TexturePack>& texPack) { return texPack->handle; }

    void Destroy(const Ref<TexturePack>& texPack)
    {
        if (ir::rdr::DestroyTexturePack_i(texPack.get()))
            ir::HF.graphicsResources.texturePacks.erase((uint64_t)texPack.get());
    }

    Ref<TexturePack> Create(const TexturePackCreationInfo& info)
    {
        Ref<TexturePack> texPack = MakeRef<TexturePack>(info);
        ir::HF.graphicsResources.texturePacks[(uint64_t)texPack.get()] = texPack;
        return texPack;
    }

    void Destroy(const Ref<TexturePack>* pPacks, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            auto pack = pPacks[i];
            if (ir::rdr::DestroyTexturePack_i(pack.get()))
                ir::HF.graphicsResources.texturePacks.erase((uint64_t)pack.get());
        }
    }

    namespace ir::rdr
    {
        template<typename T>
        static void LoadBindings(c4::yml::NodeRef root, AssetType assetType, List<TexturePackBindingInfo<T>>& ls, List<typename TexturePackBindingInfo<T>::TextureInfo>& infoLs)
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

            List<char> metadata{};
            START_READING(assetLoc.c_str(), metadata);

            TexturePackCreationInfo info{};

            ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
            ryml::NodeRef root = tree.rootref();

            List<TexturePackBindingInfo<Texture>> texList{};
            List<TexturePackBindingInfo<Texture>::TextureInfo> texInfoList{};
            List<TexturePackBindingInfo<Cubemap>> cubList{};
            List<TexturePackBindingInfo<Cubemap>::TextureInfo> cubInfoList{};
            List<TexturePackBindingInfo<RenderTexture>> rtList{};
            List<TexturePackBindingInfo<RenderTexture>::TextureInfo> rtInfoList{};

            {
                {
                    const auto v = root["layout"].val();
                    const std::string_view vView{v.str, v.len};
                    info.layout = FindTextureLayout(vView);
                }

                LoadBindings(root["texBindings"], AssetType::Texture, texList, texInfoList);
                LoadBindings(root["cubBindings"], AssetType::Cubemap, cubList, cubInfoList);
                LoadBindings(root["rtBindings"], AssetType::Texture, rtList, rtInfoList);
            }

            info.pTextureBindings = texList.data();
            info.textureBindingCount = (uint32_t)texList.size();
            info.pCubemapBindings = cubList.data();
            info.cubemapBindingCount = (uint32_t)cubList.size();
            info.pRenderTextureBindings = rtList.data();
            info.renderTextureBindingCount = (uint32_t)rtList.size();

            return MakeRef<TexturePack>(info);
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
