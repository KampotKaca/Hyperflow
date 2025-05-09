#ifndef HTEXTUREPACKALLOCATOR_H
#define HTEXTUREPACKALLOCATOR_H

#include "hshared.h"

namespace hf
{
    struct TexturePackAllocator
    {
        TexturePackAllocator(const TexturePackAllocatorCreationInfo& info);
        ~TexturePackAllocator();

        std::vector<Ref<TexturePack>> texturePacks{};
        void* handle{};
    };
}

#endif //HTEXTUREPACKALLOCATOR_H
