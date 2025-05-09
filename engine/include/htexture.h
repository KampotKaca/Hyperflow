#ifndef HTEXTURE_H
#define HTEXTURE_H

#include "hshared.h"

namespace hf
{
    struct Texture
    {
        Texture(const TextureCreationInfo& info);
        ~Texture();

        std::string filePath{};
        TextureType type = TextureType::Tex2D;
        BufferMemoryType memoryType = BufferMemoryType::Static;
        TextureFormat format = TextureFormat::R8G8B8A8_Srgb;
        TextureChannel desiredChannel = TextureChannel::RGBA;
        uint32_t mipLevels{};
        void* pixelCache{};

        void* handle{};
    };
}

#endif //HTEXTURE_H
