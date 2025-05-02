#ifndef HVK_UNIFORMALLOCATOR_H
#define HVK_UNIFORMALLOCATOR_H

#include "hvk_shared.h"

namespace hf
{
    struct VkUniformAllocator
    {
        VkUniformAllocator(const UniformAllocatorDefinitionInfo& info);
        ~VkUniformAllocator();

        VkDescriptorPool pool{};
    };

    bool IsValidAllocator(UniformAllocator allocator);
    const VkUniformAllocator& GetAllocator(UniformAllocator allocator);
}

#endif //HVK_UNIFORMALLOCATOR_H
