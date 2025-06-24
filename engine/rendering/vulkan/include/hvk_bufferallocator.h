#ifndef HVK_UNIFORMALLOCATOR_H
#define HVK_UNIFORMALLOCATOR_H

#include "hvk_shared.h"

namespace hf
{
    struct VkBufferAllocator
    {
        explicit VkBufferAllocator(const BufferAllocatorDefinitionInfo& info);
        ~VkBufferAllocator();

        VkDescriptorPool pool{};
    };

    bool IsValidAllocator(BufferAllocator allocator);
    URef<VkBufferAllocator>& GetAllocator(BufferAllocator allocator);
}

#endif //HVK_UNIFORMALLOCATOR_H
