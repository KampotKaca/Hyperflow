#ifndef HTEXTUREPACK_H
#define HTEXTUREPACK_H

#include "hshared.h"

namespace hf
{
    struct TexturePack
    {
        TexturePack(const TexturePackCreationInfo& info);
        ~TexturePack();

        struct TexturePackUploadInfo
        {
            std::string filePath;
            TextureFormat format;
            TextureType type;
            TextureChannel desiredChannel;
        };

        TexturePackUploadInfo* pUploadInfos{};
        uint32_t textureCount{};
        void* handle{};
    };
}

#endif //HTEXTUREPACK_H
