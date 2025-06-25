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
