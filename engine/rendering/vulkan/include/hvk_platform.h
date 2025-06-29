#ifndef HVK_PLATFORM_H
#define HVK_PLATFORM_H

#include "hshared.h"
#include <vulkan/vulkan.h>

namespace hf
{
    struct VulkanPlatformAPI
    {
        const char* requiredExtension[16]{};
        const char* deviceExtension[16]{};
        uint32_t requiredExtensionCount = 0;
        uint32_t deviceExtensionCount = 0;
    };

    VulkanPlatformAPI* GetAPI();
}

#endif //HVK_PLATFORM_H
