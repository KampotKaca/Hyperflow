#ifndef HYPERFLOW_HASSET_H
#define HYPERFLOW_HASSET_H

#include "hshared.h"

namespace hf
{
    struct Asset
    {
        AssetType type = AssetType::None;
        Ref<void> asset{};
        uint32_t usageCount = 0;

        void Increment(const char* assetPath);
        void Decrement();
        void Destroy();
        void Create(const char* assetPath);
    };
}

#endif //HYPERFLOW_HASSET_H
