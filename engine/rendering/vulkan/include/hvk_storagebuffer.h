#ifndef HVK_SHADERSTORAGE_H
#define HVK_SHADERSTORAGE_H

#include "hvk_shared.h"
#include "hvk_boundbuffer.h"

namespace hf
{
    struct VkStorageBuffer : public VkBoundBuffer
    {
        explicit VkStorageBuffer(const StorageBufferDefinitionInfo& info);
        ~VkStorageBuffer() override = default;
    };
}

#endif //HVK_SHADERSTORAGE_H
