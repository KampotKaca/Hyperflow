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

        uint8_t* bufferMemory{};
        uint32_t bufferIndex{}, sizeInBytes{};
        uvec3 octreePosition{};
        bool isMarkedAsModified = false;
    };
}

#endif //HMATERIAL_H
