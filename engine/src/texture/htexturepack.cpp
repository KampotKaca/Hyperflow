#include "htexturepack.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "htexture.h"

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

    TexturePack::TexturePack(const TexturePackCreationInfo& info) : layout(info.layout)
    {
        InitTextures(textureBindings, info.pTextureBindings, info.textureBindingCount);
        InitTextures(cubemapBindings, info.pCubemapBindings, info.cubemapBindingCount);
        InitTextures(renderTextureBindings, info.pRenderTextureBindings, info.renderTextureBindingCount);

        inter::rendering::CreateTexturePack_i(this);
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

    void SubmitAllTexturePacks() { inter::HF.renderingApi.api.SubmitTextureCopyOperations(); }
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
        static void PassTextures(StaticVector<TexturePack::Binding<T>, 8>& src,
            StaticVector<TexturePackBindInfo::TextureInfo, MAX_TEXTURES_IN_TEXTURE_PACK * MAX_TEXTURES_IN_TEXTURE_PACK * 3>& texInfos,
            StaticVector<TexturePackBindInfo, MAX_TEXTURES_IN_TEXTURE_PACK * 3>& texPackBindInfos,
            TexturePackBindingType type)
        {
            for (uint32_t i = 0; i < src.size(); i++)
            {
                auto& binding = src.at(i);
                uint32_t from = texInfos.size();
                for (uint32_t j = 0; j < binding.textures.size(); j++)
                {
                    auto& bi = binding.textures.atC(j);
                    texInfos.push_back({
                        .type = type,
                        .texture = bi.texture->handle,
                        .index = bi.index
                    });
                }

                texPackBindInfos.push_back({
                    .sampler = binding.sampler,
                    .textures = texInfos.atP(from),
                    .arraySize = (uint32_t)(texInfos.size() - from),
                    .bindingIndex = binding.bindingIndex
                });
            }
        }

        bool CreateTexturePack_i(TexturePack* texPack)
        {
            if (texPack->handle) return false;

            StaticVector<TexturePackBindInfo::TextureInfo, MAX_TEXTURES_IN_TEXTURE_PACK * MAX_TEXTURES_IN_TEXTURE_PACK * 3> texInfos{};
            StaticVector<TexturePackBindInfo, MAX_TEXTURES_IN_TEXTURE_PACK * 3> texPackBindInfos{};

            PassTextures(texPack->textureBindings, texInfos, texPackBindInfos, TexturePackBindingType::Texture2D);
            PassTextures(texPack->cubemapBindings, texInfos, texPackBindInfos, TexturePackBindingType::Cubemap);
            PassTextures(texPack->renderTextureBindings, texInfos, texPackBindInfos, TexturePackBindingType::RenderTexture);


            const TexturePackCreationInfo info
            {
                .layout = texPack->layout,
                .bindingCount = (uint32_t)texPackBindInfos.size(),
                .pBindings = texPackBindInfos.atP(0),
            };

            texPack->handle = HF.renderingApi.api.CreateTexturePack(info);
            return true;
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
