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
            std::string filePath{};
            TextureFormat format = TextureFormat::R8G8B8A8_Srgb;
            TextureChannel desiredChannel = TextureChannel::RGBA;
        };

        TexturePackUploadInfo* pUploadInfos{};
        uint32_t textureCount{};
        TextureType type = TextureType::Tex2D;
        BufferMemoryType memoryType = BufferMemoryType::Static;
        void* handle{};
    };
}

#endif //HTEXTUREPACK_H
