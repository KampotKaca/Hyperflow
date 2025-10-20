#ifndef HTEXTURE_H
#define HTEXTURE_H

#include "hshared.h"

namespace hf
{
    struct Texture
    {
        explicit Texture(const TextureCreationInfo& info);
        ~Texture();

        TextureChannel channels = TextureChannel::RGBA;
        TextureDetails details{};
        uint32_t mipLevels{};
        uvec3 size{};

        void* handle{};
    };
}

#endif //HTEXTURE_H
