#include "htexturepack.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "htexture.h"

namespace hf
{
    TexturePack::TexturePack(const TexturePackCreationInfo& info)
        : bindingId(info.bindingId), layout(info.layout), bindingType(info.bindingType), setBindingIndex(info.setBindingIndex)
    {
        bindings = std::vector<TexturePackBinding>(info.bindingCount);
        for (uint32_t i = 0; i < info.bindingCount; i++)
        {
            auto& bInfo = info.pBindings[i];
            auto& binding = bindings[i];

            binding.sampler = bInfo.sampler;
            binding.textures = std::vector<Ref<Texture>>(bInfo.arraySize);
            for (uint32_t j = 0; j < bInfo.arraySize; j++) binding.textures[j] = bInfo.pTextures[j];
        }

        inter::rendering::CreateTexturePack_i(this);
    }

    TexturePack::~TexturePack()
    {
        bindings.clear();
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

        void SetBindingSampler(const Ref<TexturePack>& pack, uint32_t bindingIndex, TextureSampler sampler)
        {
            auto& binding = pack->bindings[bindingIndex];
            binding.sampler = sampler;

            inter::rendering::TexturePackUploadInfo uploadInfo
            {
                .bindingIndex = bindingIndex,
                .pTextures = nullptr,
                .textureOffset = 0,
                .textureCount = 0,
                .sampler = binding.sampler
            };
            inter::HF.renderingApi.api.UploadTexturePack(pack->handle, uploadInfo);
        }

        void SetBindingTextures(const Ref<TexturePack>& pack, uint32_t bindingIndex,
            const Ref<Texture>* pTextures, uint32_t textureCount, uint32_t textureOffset)
        {
            if (textureCount == 0) return;
            auto& binding = pack->bindings[bindingIndex];
            uint32_t end = textureOffset + textureCount;
            for (uint32_t i = textureOffset; i < end; i++) binding.textures[i] = pTextures[i];

            inter::rendering::TexturePackUploadInfo uploadInfo
            {
                .bindingIndex = bindingIndex,
                .textureOffset = textureOffset,
                .textureCount = textureCount,
                .sampler = binding.sampler
            };

            std::vector<void*> texHandles(textureCount);
            for (uint32_t i = 0; i < textureCount; i++)
                texHandles[i] = binding.textures[textureOffset + i]->handle;
            uploadInfo.pTextures = texHandles.data();
            inter::HF.renderingApi.api.UploadTexturePack(pack->handle, uploadInfo);
        }

        void SetBinding(const Ref<TexturePack>& pack, uint32_t bindingIndex, TextureSampler sampler,
            const Ref<Texture>* pTextures, uint32_t textureCount, uint32_t textureOffset)
        {
            auto& binding = pack->bindings[bindingIndex];
            for (uint32_t i = 0; i < textureCount; i++) binding.textures[textureOffset] = pTextures[i];
            binding.sampler = sampler;

            inter::rendering::TexturePackUploadInfo uploadInfo
            {
                .bindingIndex = bindingIndex,
                .textureOffset = textureOffset,
                .textureCount = textureCount,
                .sampler = binding.sampler
            };

            if (textureCount > 0)
            {
                std::vector<void*> texHandles(textureCount);
                for (uint32_t i = 0; i < textureCount; i++)
                    texHandles[i] = binding.textures[textureOffset + i]->handle;
                uploadInfo.pTextures = texHandles.data();
            }
            else uploadInfo.pTextures = nullptr;
            inter::HF.renderingApi.api.UploadTexturePack(pack->handle, uploadInfo);
        }

        void Bind(const Ref<Renderer>& rn, const Ref<TexturePack>& pack)
        {
            inter::HF.renderingApi.api.BindTexturePack(rn->handle, pack->handle);
        }
    }

    namespace inter::rendering
    {
        bool CreateTexturePack_i(TexturePack* texPack)
        {
            if (texPack->handle) return false;

            std::vector<TexturePackBinding> bindings(texPack->bindings.size());

            for (uint32_t i = 0; i < texPack->bindings.size(); i++)
            {
                auto& bInfo = texPack->bindings[i];
                auto& binding = bindings[i];
                binding =
                {
                    .sampler = bInfo.sampler,
                    .textures = std::vector<void*>(bInfo.textures.size())
                };

                for (uint32_t j = 0; j < bInfo.textures.size(); j++)
                    binding.textures[j] = bInfo.textures[j] ? bInfo.textures[j]->handle : nullptr;
            }

            TexturePackCreationInfo creationInfo
            {
                .bindingType = texPack->bindingType,
                .bindingId = texPack->bindingId,
                .setBindingIndex = texPack->setBindingIndex,
                .pBindings = bindings.data(),
                .bindingCount = (uint32_t)bindings.size(),
                .layout = texPack->layout
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
                return true;
            }
            return false;
        }
    }
}
