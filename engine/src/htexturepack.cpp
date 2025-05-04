#include <stb_image.h>
#include "htexturepack.h"
#include "hinternal.h"
#include "hyperflow.h"

namespace hf
{
    TexturePack::TexturePack(const TexturePackCreationInfo& info)
    {
        textureCount = info.textureCount;
        pUploadInfos = (TexturePackUploadInfo*)utils::Allocate(info.textureCount * sizeof(TexturePackUploadInfo));

        for (uint32_t i = 0; i < info.textureCount; i++)
        {
            auto& uploadInfo = pUploadInfos[i];
            auto& inputInfo = info.pTextures[i];
            uploadInfo.format = inputInfo.format;
            uploadInfo.type = inputInfo.type;
            uploadInfo.desiredChannel = inputInfo.desiredChannel;
            uploadInfo.filePath = std::string(inputInfo.filePath);
        }
    }

    TexturePack::~TexturePack()
    {
        utils::Deallocate(pUploadInfos);
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
                    creationInfo.size = size;
                    creationInfo.channel = (TextureChannel)texChannels;
                    creationInfo.data = pixels;
                    validTextureCount++;
                }
                else creationInfos[i] = {};
            }

            if (validTextureCount == 0) return false;

            TexturePackCreationInfo creationInfo
            {
                .pTextures = creationInfos.data(),
                .textureCount = (uint32_t)creationInfos.size()
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
