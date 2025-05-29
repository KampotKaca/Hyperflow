#ifndef HVK_UNIFORMALLOCATOR_H
#define HVK_UNIFORMALLOCATOR_H

#include "hvk_shared.h"

namespace hf
{
    struct VkUniformAllocator
    {
        explicit VkUniformAllocator(const UniformAllocatorDefinitionInfo& info);
        ~VkUniformAllocator();
        VkUniformAllocator(VkUniformAllocator&& other) noexcept;

        VkDescriptorPool pool{};
    };

    bool IsValidAllocator(UniformAllocator allocator);
    const VkUniformAllocator& GetAllocator(UniformAllocator allocator);
}

#endif //HVK_UNIFORMALLOCATOR_H
