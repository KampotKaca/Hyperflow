#include "hvk_platform.h"
#include "../../../platforms/windows/include/hwin_shared.h"
#include <vulkan/vulkan_win32.h>

namespace hf
{
#if DEBUG
#define NUM_REQUIRED_EXTENSIONS 3
#else
#define NUM_REQUIRED_EXTENSIONS 2
#endif
#define NUM_DEVICE_EXTENSIONS 1

    const char* REQUIRED_EXTENSIONS[]
    {
        VK_KHR_SURFACE_EXTENSION_NAME,
        "VK_KHR_win32_surface",

#if DEBUG
        "VK_EXT_debug_utils"
#endif
    };

    const char* DEVICE_EXTENSIONS[]
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    void CreateSurface(void* platformHandle, void* winHandle, VkInstance instance, VkSurfaceKHR* result)
    {
        VkWin32SurfaceCreateInfoKHR createInfo
        {
            .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .hinstance = (HINSTANCE)platformHandle,
            .hwnd = (HWND)winHandle,
        };

        auto res = vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, result);
        if (res != VK_SUCCESS) throw std::runtime_error("Failed to create surface!");
    }

    API VulkanPlatformAPI* GetAPI()
    {
        static VulkanPlatformAPI api =
        {
            .CreateSurface = CreateSurface,
            .requiredExtensionCount = NUM_REQUIRED_EXTENSIONS,
            .deviceExtensionCount = NUM_DEVICE_EXTENSIONS
        };

        for (uint32_t i = 0; i < NUM_REQUIRED_EXTENSIONS; i++) api.requiredExtension[i] = REQUIRED_EXTENSIONS[i];
        for (uint32_t i = 0; i < NUM_DEVICE_EXTENSIONS; i++) api.deviceExtension[i] = DEVICE_EXTENSIONS[i];

        return &api;
    }
}