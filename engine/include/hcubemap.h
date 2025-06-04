#ifndef HCUBEMAP_H
#define HCUBEMAP_H

#include "hshared.h"

namespace hf
{
    struct Cubemap
    {
        explicit Cubemap(const CubemapCreationInfo& info);
        ~Cubemap();

        std::string folderPath{};
        Ref<Texture> textures[6];
    };
}

#endif //HCUBEMAP_H
