#include "hvk_platform.h"

namespace hf
{
#if DEBUG
#define NUM_REQUIRED_EXTENSIONS 4
#else
#define NUM_REQUIRED_EXTENSIONS 3
#endif
#define NUM_DEVICE_EXTENSIONS 3

    const char* REQUIRED_EXTENSIONS[]
    {
        VK_KHR_SURFACE_EXTENSION_NAME,
        "VK_KHR_wayland_surface",
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
#if DEBUG
        "VK_EXT_debug_utils"
#endif
    };

    const char* DEVICE_EXTENSIONS[]
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
        VK_KHR_MAINTENANCE3_EXTENSION_NAME,
    };

    VulkanPlatformAPI* GetAPI()
    {
        static VulkanPlatformAPI api =
        {
            .requiredExtensionCount = NUM_REQUIRED_EXTENSIONS,
            .deviceExtensionCount = NUM_DEVICE_EXTENSIONS
        };

        for (uint32_t i = 0; i < NUM_REQUIRED_EXTENSIONS; i++) api.requiredExtension[i] = REQUIRED_EXTENSIONS[i];
        for (uint32_t i = 0; i < NUM_DEVICE_EXTENSIONS; i++) api.deviceExtension[i] = DEVICE_EXTENSIONS[i];

        return &api;
    }
}