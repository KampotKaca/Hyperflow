#ifndef HTEXTUREALLOCATOR_H
#define HTEXTUREALLOCATOR_H

#include "hshared.h"

namespace hf
{
    struct TextureAllocator
    {
        TextureAllocator(const TextureAllocatorCreationInfo& info);
        ~TextureAllocator();

        std::vector<Ref<Texture>> textures{};
        void* handle{};
    };
}

#endif //HTEXTUREALLOCATOR_H
