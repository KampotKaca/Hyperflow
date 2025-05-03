#ifndef HTEXTUREPACK_H
#define HTEXTUREPACK_H

#include "hshared.h"

namespace hf
{
    struct TexturePack
    {
        TexturePack(const TexturePackCreationInfo& info);
        ~TexturePack();

        TexturePackCreationInfo creationInfo{};
        void* handle{};
    };
}

#endif //HTEXTUREPACK_H
