#include "hvk_platform.h"

namespace hf
{
    std::array REQUIRED_EXTENSIONS
    {
        VK_KHR_SURFACE_EXTENSION_NAME,
        "VK_KHR_win32_surface",
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
#if DEBUG
        "VK_EXT_debug_utils"
#endif
    };

    std::array DEVICE_EXTENSIONS
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
        VK_KHR_MAINTENANCE3_EXTENSION_NAME,
        VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
        VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
        VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
        VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,
        VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME
    };

    VulkanPlatformAPI* GetAPI()
    {
        static VulkanPlatformAPI api =
        {
            .requiredExtensionCount = REQUIRED_EXTENSIONS.size(),
            .deviceExtensionCount = DEVICE_EXTENSIONS.size()
        };

        for (uint32_t i = 0; i < REQUIRED_EXTENSIONS.size(); i++) api.requiredExtension[i] = REQUIRED_EXTENSIONS[i];
        for (uint32_t i = 0; i < DEVICE_EXTENSIONS.size(); i++) api.deviceExtension[i] = DEVICE_EXTENSIONS[i];

        return &api;
    }
}