#include "htexturepackallocator.h"
#include "hinternal.h"
#include "htexturepack.h"

namespace hf
{
    TexturePackAllocator::TexturePackAllocator(const TexturePackAllocatorCreationInfo& info)
    {
        texturePacks = std::vector<Ref<TexturePack>>(info.texturePackCount);
        memcpy(texturePacks.data(), info.pTexturePacks, info.texturePackCount * sizeof(Ref<TexturePack>));
        inter::rendering::CreateTexturePackAllocator_i(this);
    }

    TexturePackAllocator::~TexturePackAllocator()
    {
        inter::rendering::DestroyTexturePackAllocator_i(this);
    }

    namespace texturepackallocator
    {
        Ref<TexturePackAllocator> Create(const TexturePackAllocatorCreationInfo& info)
        {
            Ref<TexturePackAllocator> texPackAllocator = MakeRef<TexturePackAllocator>(info);
            inter::HF.graphicsResources.texturePackAllocators[(uint64_t)texPackAllocator.get()] = texPackAllocator;
            return texPackAllocator;
        }

        void Destroy(const Ref<TexturePackAllocator>& texPackAllocator)
        {
            if (inter::rendering::DestroyTexturePackAllocator_i(texPackAllocator.get()))
                inter::HF.graphicsResources.texturePackAllocators.erase((uint64_t)texPackAllocator.get());
        }

        void Destroy(const Ref<TexturePackAllocator>* pTexPackAllocators, uint32_t count)
        {
            for (uint32_t i = 0; i < count; i++)
            {
                auto packAllocator = pTexPackAllocators[i];
                if (inter::rendering::DestroyTexturePackAllocator_i(packAllocator.get()))
                    inter::HF.graphicsResources.texturePackAllocators.erase((uint64_t)packAllocator.get());
            }
        }

        void DestroyAll(bool internalOnly = false)
        {
            for (const auto& packAllocator : std::ranges::views::values(inter::HF.graphicsResources.texturePackAllocators))
                inter::rendering::DestroyTexturePackAllocator_i(packAllocator.get());
            if (!internalOnly) inter::HF.graphicsResources.texturePackAllocators.clear();
        }

        bool IsRunning(const Ref<TexturePackAllocator>& texPackAllocator) { return texPackAllocator->handle; }
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
                HF.renderingApi.api.DestroyTexturePackAllocator(packAllocator->handle);
                packAllocator->handle = nullptr;
                return true;
            }
            return false;
        }
    }
}
