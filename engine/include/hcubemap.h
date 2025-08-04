#ifndef HCUBEMAP_H
#define HCUBEMAP_H

#include "hshared.h"

namespace hf
{
    struct Cubemap
    {
        explicit Cubemap(const CubemapCreationInfo& info);
        ~Cubemap();

        FilePath folderPath{};
        FilePath texturePaths[6]{};
        TextureChannel desiredChannel = TextureChannel::RGBA;
        uint32_t mipLevels = 1;
        TextureDetails details{};

        void* handle{};
    };
}

#endif //HCUBEMAP_H
