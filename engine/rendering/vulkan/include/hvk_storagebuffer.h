#ifndef HVK_SHADERSTORAGE_H
#define HVK_SHADERSTORAGE_H

#include "hvk_shared.h"

namespace hf
{
    struct VkStorageBuffer
    {
        explicit VkStorageBuffer(const StorageBufferCreationInfo& info);
        ~VkStorageBuffer();

        uint32_t bindingId{};
        uint64_t bufferSize{};
        BufferMemoryType memoryType = BufferMemoryType::Static;
        void* mapping{};

        VkBuffer buffer{};
        VmaAllocation bufferMemory{};
    };
}

#endif //HVK_SHADERSTORAGE_H
