#ifndef HTEXTUREPACKALLOCATOR_H
#define HTEXTUREPACKALLOCATOR_H

#include "hshared.h"

namespace hf
{
    struct TexturePackAllocator
    {
        explicit TexturePackAllocator(const TexturePackAllocatorCreationInfo& info);
        ~TexturePackAllocator();

        [[nodiscard]] bool IsRunning() const;
        void Destroy();

        static Ref<TexturePackAllocator> Create(const TexturePackAllocatorCreationInfo& info);
        static void Destroy(const Ref<TexturePackAllocator>* pTexPackAllocators, uint32_t count);

#ifndef HF_ENGINE_INTERNALS
    private:
#endif
        std::vector<Ref<TexturePack>> texturePacks{};
        void* handle{};
    };
}

#endif //HTEXTUREPACKALLOCATOR_H
