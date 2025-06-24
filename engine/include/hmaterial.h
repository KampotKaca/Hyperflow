#ifndef HMATERIAL_H
#define HMATERIAL_H

#include "hshared.h"
#include "../config.h"

namespace hf
{
    struct Material
    {
        explicit Material(const MaterialCreationInfo& info);
        ~Material();

        [[nodiscard]] uint16_t GetBufferIndex() const { return bufferIndex; }

        void Upload(const void* data);
        //No need to destroy the material, if it goes out of scope it is automatically freed!
        static Ref<Material> Create(const MaterialCreationInfo& info);

#ifndef HF_ENGINE_INTERNALS
    private:
#endif

        uint8_t* bufferMemory{};
        uint32_t bufferIndex{}, sizeInBytes{};
        uvec3 octreePosition{};
        bool isMarkedAsModified = false;
    };
}

#endif //HMATERIAL_H
