#define HF_ENGINE_INTERNALS
#include "htexturepack.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "htexture.h"

namespace hf
{
    TexturePack::TexturePack(const TexturePackCreationInfo& info)
        : bindingType(info.bindingType), textureBindingCount(info.textureBindingCount),
          cubemapBindingCount(info.cubemapBindingCount),
          layout(info.layout), bindingId(info.bindingId)
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

        bindingsBuffer = std::vector<inter::rendering::TexturePackBindingCreationInfo>(bindings.size());
        inter::rendering::CreateTexturePack_i(this);
    }

    TexturePack::~TexturePack()
    {
        bindings.clear();
        inter::rendering::DestroyTexturePack_i(this);
    }

    bool TexturePack::IsRunning() const { return handle; }

    void TexturePack::Destroy()
    {
        if (inter::rendering::DestroyTexturePack_i(this))
            inter::HF.graphicsResources.texturePacks.erase((uint64_t)this);
    }

    template<typename T>
    void TexturePack::SetBinding(const TexturePackBindingUploadInfo<T>& info)
    {
        static std::vector<void*> textures(32);
        auto& binding = bindings[info.bindingIndex];

        inter::rendering::TexturePackBindingUploadInfo uploadInfo
        {
            .bindingIndex = info.bindingIndex,
            .sampler = info.sampler,
        };

        if (info.sampler.has_value()) binding.sampler = info.sampler.value();
        if (info.texInfo.has_value())
        {
            auto& texUpload = info.texInfo.value();
            for (uint32_t i = 0; i < texUpload.count; i++)
                binding.textures[texUpload.offset + i] = texUpload.pTextures[i]->handle;

            if (texUpload.count > 0)
            {
                textures.clear();
                inter::rendering::TexturePackTextureUploadInfo texUploadInfo
                {
                    .offset = texUpload.offset,
                    .count = texUpload.count
                };

                for (uint32_t i = 0; i < texUpload.count; i++)
                    textures.push_back(binding.textures[texUpload.offset + i]);
                texUploadInfo.pTextures = textures.data();

                uploadInfo.texInfo = texUploadInfo;
            }
        }

        inter::HF.renderingApi.api.UploadTexturePackBinding(handle, uploadInfo);
    }

    void TexturePack::SubmitAll() { inter::HF.renderingApi.api.SubmitTextureCopyOperations(); }
    Ref<TexturePack> TexturePack::Create(const TexturePackCreationInfo& info)
    {
        Ref<TexturePack> texPack = MakeRef<TexturePack>(info);
        inter::HF.graphicsResources.texturePacks[(uint64_t)texPack.get()] = texPack;
        return texPack;
    }

    void TexturePack::Destroy(const Ref<TexturePack>* pPacks, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            auto pack = pPacks[i];
            if (inter::rendering::DestroyTexturePack_i(pack.get()))
                inter::HF.graphicsResources.texturePacks.erase((uint64_t)pack.get());
        }
    }

    template void TexturePack::SetBinding<Texture>(const TexturePackBindingUploadInfo<Texture>&);
    template void TexturePack::SetBinding<Cubemap>(const TexturePackBindingUploadInfo<Cubemap>&);

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
                bindingBuffer.count = binding.textures.size();
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
