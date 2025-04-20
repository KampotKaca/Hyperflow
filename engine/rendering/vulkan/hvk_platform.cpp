#include "hvk_graphics.h"
#include "hvk_renderer.h"
#include "exceptions/hgraphicsexception.h"
#include "hgenericexception.h"

// #ifdef VkResult
// #undef VkResult
// #endif

#if PLATFORM_WINDOWS
#include "../platforms/windows/hwin_shared.h"
#include <vulkan/vulkan_win32.h>
#elif PLATFORM_LINUX

    #if X11
    #include "../platforms/linux/x11/hlnx_window.h"
    #include <vulkan/vulkan_xlib.h>
    #elif WAYLAND

    #endif

#endif

namespace hf::inter::rendering
{
    const char* REQUIRED_EXTENSIONS[NUM_REQUIRED_EXTENSIONS] =
    {
        VK_KHR_SURFACE_EXTENSION_NAME,
#if PLATFORM_WINDOWS
        "VK_KHR_win32_surface",
#elif PLATFORM_LINUX
    #if X11
        "VK_KHR_xlib_surface",
    #elif WAYLAND
        "VK_KHR_wayland_surface",
    #endif
#endif

#if DEBUG
        "VK_EXT_debug_utils"
#endif
    };

    const char* DEVICE_EXTENSIONS[NUM_DEVICE_EXTENSIONS] =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

#if PLATFORM_WINDOWS

    void CreateSurface(VKRenderer* rendererData)
    {
        VkWin32SurfaceCreateInfoKHR createInfo
        {
            .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .hinstance = PLATFORM_DATA.instance,
            .hwnd = (HWND)rendererData->windowHandle,
        };

        VK_HANDLE_EXCEPT(vkCreateWin32SurfaceKHR(GRAPHICS_DATA.instance, &createInfo, nullptr, &rendererData->swapchain.surface));
    }

#elif PLATFORM_LINUX

#if X11

    void CreateSurface(VKRenderer* rn)
    {
        VkXlibSurfaceCreateInfoKHR createInfo
        {
            .sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
            .dpy = PLATFORM_DATA.display,
            .window = ((LnxWindowData*)rn->windowHandle)->window,
        };

        VK_HANDLE_EXCEPT(vkCreateXlibSurfaceKHR(GRAPHICS_DATA.instance, &createInfo, nullptr, &rn->swapchain.surface));
    }

#elif WAYLAND

    void CreateSurface(VKRenderer* rendererData)
    {

    }

#endif

#endif

    void DestroySurface(VKRenderer* rn)
    {
        vkDestroySurfaceKHR(GRAPHICS_DATA.instance, rn->swapchain.surface, nullptr);
        rn->swapchain.surface = VK_NULL_HANDLE;
    }
}