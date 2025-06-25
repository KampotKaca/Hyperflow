#ifndef HCUBEMAP_H
#define HCUBEMAP_H

#include "hshared.h"

namespace hf
{
    struct Cubemap
    {
        explicit Cubemap(const CubemapCreationInfo& info);
        ~Cubemap();

        bool IsRunning() const;
        void Destroy();

        static Ref<Cubemap> Create(const CubemapCreationInfo& info);
        static Ref<Cubemap> Create(const char* assetPath);
        static void Destroy(const Ref<Cubemap>* pCubemaps, uint32_t count);

        std::string folderPath{};
        std::string texturePaths[6]{};
        TextureChannel desiredChannel = TextureChannel::RGBA;
        uint32_t mipLevels = 1;
        TextureDetails details{};

        void* handle;
    };
}

#endif //HCUBEMAP_H
