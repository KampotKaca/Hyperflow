#include <stb_image.h>
#include "htexturepack.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "htexture.h"

namespace hf
{
    TexturePack::TexturePack(const TexturePackCreationInfo& info)
    {
        bindingId = info.bindingId;
        usageStage = info.usageStage;
        textures = std::vector<Ref<Texture>>(info.textureCount);
        memcpy(textures.data(), info.pTextures, info.textureCount * sizeof(Ref<Texture>));

        inter::rendering::CreateTexturePack_i(this);
    }

    TexturePack::~TexturePack()
    {
        inter::rendering::DestroyTexturePack_i(this);
    }

    namespace texturepack
    {
        Ref<TexturePack> Create(const TexturePackCreationInfo& info)
        {
            Ref<TexturePack> texPack = MakeRef<TexturePack>(info);
            inter::HF.graphicsResources.texturePacks[texPack.get()] = texPack;
            return texPack;
        }

        void Destroy(const Ref<TexturePack>& pack)
        {
            if (inter::rendering::DestroyTexturePack_i(pack.get()))
                inter::HF.graphicsResources.texturePacks.erase(pack.get());
        }

        void Destroy(const Ref<TexturePack>* pPacks, uint32_t count)
        {
            for (uint32_t i = 0; i < count; i++)
            {
                auto pack = pPacks[i];
                if (inter::rendering::DestroyTexturePack_i(pack.get()))
                    inter::HF.graphicsResources.texturePacks.erase(pack.get());
            }
        }

        void DestroyAll(bool internalOnly)
        {
            for (const auto& pack : std::ranges::views::values(inter::HF.graphicsResources.texturePacks))
                inter::rendering::DestroyTexturePack_i(pack.get());
            if (!internalOnly) inter::HF.graphicsResources.texturePacks.clear();
        }

        bool IsRunning(const Ref<TexturePack>& pack) { return pack->handle; }
    }

    namespace inter::rendering
    {
        bool CreateTexturePack_i(TexturePack* texPack)
        {
            if (texPack->handle) return false;

            std::vector<void*> texHandles(texPack->textures.size());
            for (uint32_t i = 0; i < texPack->textures.size(); i++)
                texHandles[i] = texPack->textures[i]->handle;

            TexturePackCreationInfo creationInfo
            {
                .pTextures = texHandles.data(),
                .textureCount = (uint32_t)texHandles.size(),
                .bindingId = texPack->bindingId,
                .usageStage = texPack->usageStage,
            };

            texPack->handle = HF.renderingApi.api.CreateTexturePack(creationInfo);

            return true;
        }

        bool DestroyTexturePack_i(TexturePack* texPack)
        {
            if (texPack->handle)
            {
                HF.renderingApi.api.DestroyTexturePack(texPack->handle);
                texPack->handle = nullptr;
                texPack->textures.clear();
                return true;
            }
            return false;
        }
    }
}
