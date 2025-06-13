#include "htexturepack.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "htexture.h"

namespace hf
{
    TexturePack::TexturePack(const TexturePackCreationInfo& info)
        : bindingType(info.bindingType), layout(info.layout), bindingId(info.bindingId)
    {
        bindings = std::vector<Binding>(info.textureBindingCount + info.cubemapBindingCount);
        for (uint32_t i = 0; i < info.textureBindingCount; i++)
        {
            auto& bInfo = info.pTextureBindings[i];
            auto& binding = bindings[i];

            binding.sampler = bInfo.sampler;
            binding.textures = std::vector<void*>(bInfo.arraySize);
            for (uint32_t j = 0; j < bInfo.arraySize; j++)
                binding.textures[j] = bInfo.pTextures[j]->handle;
        }

        for (uint32_t i = 0; i < info.cubemapBindingCount; i++)
        {
            auto& bInfo = info.pCubemapeBindings[i];
            auto& binding = bindings[info.textureBindingCount + i];

            binding.sampler = bInfo.sampler;
            binding.textures = std::vector<void*>(bInfo.arraySize);
            for (uint32_t j = 0; j < bInfo.arraySize; j++)
                binding.textures[j] = bInfo.pCubemaps[j]->handle;
        }

        bindingsBuffer = std::vector<inter::rendering::TexturePackTextureBindingInfo>(bindings.size());
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
            inter::HF.graphicsResources.texturePacks[(uint64_t)texPack.get()] = texPack;
            return texPack;
        }

        void Destroy(const Ref<TexturePack>& pack)
        {
            if (inter::rendering::DestroyTexturePack_i(pack.get()))
                inter::HF.graphicsResources.texturePacks.erase((uint64_t)pack.get());
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

        bool IsRunning(const Ref<TexturePack>& pack) { return pack->handle; }

        void SetBindingSampler(const Ref<TexturePack>& pack, uint32_t bindingIndex, TextureSampler sampler)
        {
            auto& binding = pack->bindings[bindingIndex];
            binding.sampler = sampler;

            const inter::rendering::TexturePackUploadInfo uploadInfo
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
            for (uint32_t i = textureOffset; i < end; i++)
                binding.textures[i] = pTextures[i]->handle;

            inter::rendering::TexturePackUploadInfo uploadInfo
            {
                .bindingIndex = bindingIndex,
                .textureOffset = textureOffset,
                .textureCount = textureCount,
                .sampler = binding.sampler
            };

            std::vector<void*> texHandles(textureCount);
            for (uint32_t i = 0; i < textureCount; i++)
                texHandles[i] = binding.textures[textureOffset + i];
            uploadInfo.pTextures = texHandles.data();
            inter::HF.renderingApi.api.UploadTexturePack(pack->handle, uploadInfo);
        }

        void SetBinding(const Ref<TexturePack>& pack, uint32_t bindingIndex, TextureSampler sampler,
            const Ref<Texture>* pTextures, uint32_t textureCount, uint32_t textureOffset)
        {
            auto& binding = pack->bindings[bindingIndex];
            for (uint32_t i = 0; i < textureCount; i++) binding.textures[textureOffset] = pTextures[i]->handle;
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
                    texHandles[i] = binding.textures[textureOffset + i];
                uploadInfo.pTextures = texHandles.data();
            }
            else uploadInfo.pTextures = nullptr;
            inter::HF.renderingApi.api.UploadTexturePack(pack->handle, uploadInfo);
        }
    }

    namespace inter::rendering
    {
        bool CreateTexturePack_i(TexturePack* texPack)
        {
            if (texPack->handle) return false;

            for (uint32_t i = 0; i < texPack->bindings.size(); i++)
            {
                auto& binding = texPack->bindings[i];
                auto& bindingBuffer = texPack->bindingsBuffer[i];
                bindingBuffer.sampler = binding.sampler;
                bindingBuffer.pTextures = binding.textures.data();
                bindingBuffer.textureCount = binding.textures.size();
            }

            const TexturePackCreationInfo creationInfo
            {
                .bindingType = texPack->bindingType,
                .bindingId = texPack->bindingId,
                .pBindings = texPack->bindingsBuffer.data(),
                .bindingCount = (uint32_t)texPack->bindingsBuffer.size(),
                .layout = texPack->layout
            };

            texPack->handle = HF.renderingApi.api.CreateTexturePack(creationInfo);
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
    }
}
