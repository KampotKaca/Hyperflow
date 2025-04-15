#include "hvk_graphics.h"
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
    const std::vector<const char*> REQUIRED_EXTENSIONS =
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

#if PLATFORM_WINDOWS

    void CreateSurface(VKRendererData* rendererData)
    {
        VkWin32SurfaceCreateInfoKHR createInfo
        {
            .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .hinstance = PLATFORM_DATA.instance,
            .hwnd = (HWND)rendererData->windowHandle,
        };

        VK_HANDLE_EXCEPT(vkCreateWin32SurfaceKHR(GRAPHICS_DATA.instance, &createInfo, nullptr, &rendererData->surface));
    }

#elif PLATFORM_LINUX

#if X11

    void CreateSurface(VKRendererData* rendererData)
    {
        VkXlibSurfaceCreateInfoKHR createInfo
        {
            .sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
            .dpy = PLATFORM_DATA.display,
            .window = ((LnxWindowData*)rendererData->windowHandle)->window,
        };

        VK_HANDLE_EXCEPT(vkCreateXlibSurfaceKHR(GRAPHICS_DATA.instance, &createInfo, nullptr, &rendererData->surface));
    }

#elif WAYLAND

    void CreateSurface(VKRendererData* rendererData)
    {

    }

#endif

#endif

    void DestroySurface(VKRendererData* rendererData)
    {
        vkDestroySurfaceKHR(GRAPHICS_DATA.instance, rendererData->surface, nullptr);
        rendererData->surface = nullptr;
    }
}