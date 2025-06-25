#ifndef HTEXTURE_H
#define HTEXTURE_H

#include "hshared.h"

namespace hf
{
    struct Texture
    {
        explicit Texture(const TextureCreationInfo& info);
        ~Texture();

        [[nodiscard]] bool IsRunning() const;
        void Destroy();

        static Ref<Texture> Create(const TextureCreationInfo& info);
        static Ref<Texture> Create(const char* assetPath);
        static void Destroy(const Ref<Texture>* pTextures, uint32_t count);

        std::string filePath{};
        bool useAbsolutePath{};
        TextureChannel desiredChannel = TextureChannel::RGBA;
        TextureDetails details{};
        uint32_t mipLevels{};
        void* pixelCache{};

        void* handle{};
    };
}

#endif //HTEXTURE_H
