#ifndef HTEXTURE_H
#define HTEXTURE_H

#include "hshared.h"

namespace hf
{
    struct Texture
    {
        explicit Texture(const TextureCreationInfo& info);
        ~Texture();

        std::string filePath{};
        TextureChannel desiredChannel = TextureChannel::RGBA;
        TextureDetails details{};
        uint32_t mipLevels{};
        void* pixelCache{};

        void* handle{};
    };
}

#endif //HTEXTURE_H
