#include <stb_image.h>
#include "htextureallocator.h"
#include "hinternal.h"
#include "htexture.h"

namespace hf
{
    TextureAllocator::TextureAllocator(const TextureAllocatorCreationInfo& info)
    {
        textures = std::vector<Ref<Texture>>(info.textureCount);
        memcpy(textures.data(), info.pTextures, info.textureCount * sizeof(Ref<Texture>));;

        inter::rendering::CreateTextureAllocator_i(this);
    }

    TextureAllocator::~TextureAllocator()
    {
        inter::rendering::DestroyTextureAllocator_i(this);
    }

    namespace textureallocator
    {
        Ref<TextureAllocator> Create(const TextureAllocatorCreationInfo& info)
        {
            Ref<TextureAllocator> texAllocator = MakeRef<TextureAllocator>(info);
            inter::HF.graphicsResources.textureAllocators[texAllocator.get()] = texAllocator;
            return texAllocator;
        }

        void Destroy(const Ref<TextureAllocator>& texAllocator)
        {
            if (inter::rendering::DestroyTextureAllocator_i(texAllocator.get()))
                inter::HF.graphicsResources.textureAllocators.erase(texAllocator.get());
        }

        void Destroy(const Ref<TextureAllocator>* pTexAllocators, uint32_t count)
        {
            for (uint32_t i = 0; i < count; i++)
            {
                auto texAllocator = pTexAllocators[i];
                if (inter::rendering::DestroyTextureAllocator_i(texAllocator.get()))
                    inter::HF.graphicsResources.textureAllocators.erase(texAllocator.get());
            }
        }

        void DestroyAll(bool internalOnly)
        {
            for (const auto& allocator : std::ranges::views::values(inter::HF.graphicsResources.textureAllocators))
                inter::rendering::DestroyTextureAllocator_i(allocator.get());
            if (!internalOnly) inter::HF.graphicsResources.textureAllocators.clear();
        }

        bool IsRunning(const Ref<TextureAllocator>& texAllocator) { return texAllocator->handle; }
    }

    namespace inter::rendering
    {
        bool CreateTextureAllocator_i(TextureAllocator* texAllocator)
        {
            if (texAllocator->handle) return false;

            std::vector<void*> texHandles(texAllocator->textures.size());
            for (uint32_t i = 0; i < texAllocator->textures.size(); i++)
                texHandles[i] = texAllocator->textures[i]->handle;

            TextureAllocatorCreationInfo creationInfo
            {
                .pTextures = texHandles.data(),
                .textureCount = (uint32_t)texHandles.size()
            };

            texAllocator->handle = HF.renderingApi.api.CreateTextureAllocator(creationInfo);
            for (uint32_t i = 0; i < texAllocator->textures.size(); i++)
            {
                stbi_image_free(texAllocator->textures[i]->pixelCache);
                texAllocator->textures[i]->pixelCache = nullptr;
            }

            return true;
        }

        bool DestroyTextureAllocator_i(TextureAllocator* texAllocator)
        {
            if (texAllocator->handle)
            {
                HF.renderingApi.api.DestroyTextureAllocator(texAllocator->handle);
                texAllocator->handle = nullptr;
                texAllocator->textures.clear();
                return true;
            }
            return false;
        }
    }
}