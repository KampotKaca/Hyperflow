#include <stb_image.h>
#include "htexturepack.h"
#include "hinternal.h"
#include "hyperflow.h"

namespace hf
{
    TexturePack::TexturePack(const TexturePackCreationInfo& info)
    {
        type = info.type;
        memoryType = info.memoryType;
        textureCount = info.textureCount;
        pUploadInfos = (TexturePackUploadInfo*)utils::Allocate(info.textureCount * sizeof(TexturePackUploadInfo));

        for (uint32_t i = 0; i < info.textureCount; i++)
        {
            new (&pUploadInfos[i]) TexturePackUploadInfo();
            auto& uploadInfo = pUploadInfos[i];
            auto& inputInfo = info.pTextures[i];
            uploadInfo.filePath = inputInfo.filePath;
            uploadInfo.format = inputInfo.format;
            uploadInfo.desiredChannel = inputInfo.desiredChannel;
            uploadInfo.sampler = inputInfo.sampler;

            if (uploadInfo.sampler == 0) throw GENERIC_EXCEPT("[Hyperflow]", "Every texture needs a sampler to read");
        }

        inter::rendering::CreateTexturePack_i(this);
    }

    TexturePack::~TexturePack()
    {
        utils::Deallocate(pUploadInfos);
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

    namespace texturesampler
    {
        TextureSampler Define(const TextureSamplerDefinitionInfo& info)
        {
            return inter::HF.renderingApi.api.DefineTextureSampler(info);
        }
    }

    namespace inter::rendering
    {
        bool CreateTexturePack_i(TexturePack* texPack)
        {
            if (texPack->handle) return false;

            std::vector<TextureCreationInfo> creationInfos(texPack->textureCount);
            uint32_t validTextureCount = 0;

            for (uint32_t i = 0; i < texPack->textureCount; i++)
            {
                std::string texLoc = TO_RES_PATH(std::string("textures/") + texPack->pUploadInfos[i].filePath);

                if (!utils::FileExists(texLoc.c_str()))
                {
                    LOG_ERROR("[Hyperflow] Unable to find texture: %s", texPack->pUploadInfos[i].filePath.c_str());
                    continue;
                }

                auto& uploadInfo = texPack->pUploadInfos[i];
                ivec3 size{};
                int32_t texChannels{};
                auto* pixels = stbi_load(texLoc.c_str(), &size.x, &size.y,
                    &texChannels, (int32_t)uploadInfo.desiredChannel);
                size.z = 1;
                if (pixels)
                {
                    auto& creationInfo = creationInfos[i];
                    creationInfo =
                    {
                        .size = size,
                        .channel = (TextureChannel)texChannels,
                        .mipLevels = 1,
                        .data = pixels,
                        .format = uploadInfo.format,
                        .sampler = uploadInfo.sampler,
                    };
                    validTextureCount++;
                }
                else creationInfos[i] = {};
            }

            if (validTextureCount == 0) return false;

            TexturePackCreationInfo creationInfo
            {
                .pTextures = creationInfos.data(),
                .textureCount = (uint32_t)creationInfos.size(),
                .type = texPack->type,
                .memoryType = texPack->memoryType,
            };

            texPack->handle = HF.renderingApi.api.CreateTexturePack(creationInfo);

            for (uint32_t i = 0; i < texPack->textureCount; i++)
            {
                if (!creationInfos[i].data) continue;
                stbi_image_free(creationInfos[i].data);
            }

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
