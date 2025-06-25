#include "htexturepackallocator.h"
#include "hinternal.h"
#include "htexturepack.h"

namespace hf
{
    TexturePackAllocator::TexturePackAllocator(const TexturePackAllocatorCreationInfo& info)
    {
        texturePacks = std::vector<Ref<TexturePack>>(info.texturePackCount);
        for (uint32_t i = 0; i < info.texturePackCount; i++) texturePacks[i] = info.pTexturePacks[i];
        inter::rendering::CreateTexturePackAllocator_i(this);
    }

    TexturePackAllocator::~TexturePackAllocator()
    {
        inter::rendering::DestroyTexturePackAllocator_i(this);
    }

    bool TexturePackAllocator::IsRunning() const { return handle; }
    void TexturePackAllocator::Destroy()
    {
        if (inter::rendering::DestroyTexturePackAllocator_i(this))
            inter::HF.graphicsResources.texturePackAllocators.erase((uint64_t)this);
    }

    Ref<TexturePackAllocator> TexturePackAllocator::Create(const TexturePackAllocatorCreationInfo& info)
    {
        Ref<TexturePackAllocator> texPackAllocator = MakeRef<TexturePackAllocator>(info);
        inter::HF.graphicsResources.texturePackAllocators[(uint64_t)texPackAllocator.get()] = texPackAllocator;
        return texPackAllocator;
    }

    void TexturePackAllocator::Destroy(const Ref<TexturePackAllocator>* pTexPackAllocators, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            auto packAllocator = pTexPackAllocators[i];
            if (inter::rendering::DestroyTexturePackAllocator_i(packAllocator.get()))
                inter::HF.graphicsResources.texturePackAllocators.erase((uint64_t)packAllocator.get());
        }
    }

    namespace inter::rendering
    {
        bool CreateTexturePackAllocator_i(TexturePackAllocator* packAllocator)
        {
            if (packAllocator->handle) return false;

            std::vector<void*> texPackHandles(packAllocator->texturePacks.size());
            for (uint32_t i = 0; i < packAllocator->texturePacks.size(); i++)
                texPackHandles[i] = packAllocator->texturePacks[i]->handle;

            TexturePackAllocatorCreationInfo creationInfo
            {
                .pTexturePacks = texPackHandles.data(),
                .texturePackCount = (uint32_t)texPackHandles.size()
            };

            packAllocator->handle = HF.renderingApi.api.CreateTexturePackAllocator(creationInfo);

            return true;
        }

        bool DestroyTexturePackAllocator_i(TexturePackAllocator* packAllocator)
        {
            if (packAllocator->handle)
            {
                std::lock_guard lock(HF.deletedResources.syncLock);
                HF.deletedResources.texturePackAllocators.push_back(packAllocator->handle);
                packAllocator->handle = nullptr;
                return true;
            }
            return false;
        }

        void DestroyAllTexturePackAllocators_i(bool internalOnly)
        {
            for (const auto& packAllocator : std::ranges::views::values(HF.graphicsResources.texturePackAllocators))
                DestroyTexturePackAllocator_i(packAllocator.get());
            if (!internalOnly) HF.graphicsResources.texturePackAllocators.clear();
        }
    }
}
