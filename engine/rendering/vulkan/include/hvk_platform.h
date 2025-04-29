#ifndef HVK_PLATFORM_H
#define HVK_PLATFORM_H

#include "hshared.h"
#include <vulkan/vulkan.h>

namespace hf
{
    struct VulkanPlatformAPI
    {
        void (*CreateSurface)(void* platformHandle, void* winHandle, VkInstance instance, VkSurfaceKHR* result);
        const char* requiredExtension[16];
        const char* deviceExtension[16];
        uint32_t requiredExtensionCount = 0;
        uint32_t deviceExtensionCount = 0;
    };
}

#endif //HVK_PLATFORM_H
