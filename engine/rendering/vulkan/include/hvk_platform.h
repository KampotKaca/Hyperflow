#ifndef HVK_PLATFORM_H
#define HVK_PLATFORM_H

#include "hshared.h"
#include <vulkan/vulkan.h>

namespace hf
{
    struct VulkanPlatformAPI
    {
        const char* requiredExtension[16]{};
        uint32_t requiredExtensionCount = 0;
    };

    constexpr std::array DEVICE_EXTENSIONS
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
        VK_KHR_MAINTENANCE3_EXTENSION_NAME,
        VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
        VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
        VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
        VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,
        VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
    };

    VulkanPlatformAPI* GetAPI();
}

#endif //HVK_PLATFORM_H
