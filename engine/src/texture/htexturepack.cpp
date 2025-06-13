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

        void SetBinding(const Ref<TexturePack>& pack, const TexturePackBindingUploadInfo& info)
        {
            auto& binding = pack->bindings[info.bindingIndex];

            inter::rendering::TexturePackUploadInfo uploadInfo
            {
                .bindingIndex = info.bindingIndex,
            };

            if (info.sampler.has_value())
            {
                binding.sampler = info.sampler.value();
                uploadInfo.sampler = binding.sampler;
            }
            else uploadInfo.sampler = binding.sampler;

            if (info.texUpload.has_value())
            {
                auto& texUpload = info.texUpload.value();
                for (uint32_t i = 0; i < texUpload.count; i++)
                    binding.textures[texUpload.offset + i] = texUpload.pTextures[i]->handle;

                if (texUpload.count > 0)
                {
                    std::vector<void*> texHandles(texUpload.count);
                    for (uint32_t i = 0; i < texUpload.count; i++)
                        texHandles[i] = binding.textures[texUpload.offset + i];
                    uploadInfo.pTextures = texHandles.data();
                }
                else uploadInfo.pTextures = nullptr;
            }
            else uploadInfo.pTextures = nullptr;

            if (info.cubemapUpload.has_value())
            {
                auto& cubemapUpload = info.cubemapUpload.value();
                for (uint32_t i = 0; i < cubemapUpload.count; i++)
                    binding.textures[cubemapUpload.offset + i] = cubemapUpload.pCubemaps[i]->handle;

                if (cubemapUpload.count > 0)
                {
                    std::vector<void*> texHandles(cubemapUpload.count);
                    for (uint32_t i = 0; i < cubemapUpload.count; i++)
                        texHandles[i] = binding.textures[cubemapUpload.offset + i];
                    uploadInfo.pTextures = texHandles.data();
                }
                else uploadInfo.pTextures = nullptr;
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
