#include "hvk_graphics.h"
#include "exceptions/hgraphicsexception.h"
#include "hgenericexception.h"

// #ifdef VkResult
// #undef VkResult
// #endif

#if PLATFORM_WINDOWS

#elif PLATFORM_LINUX

    #if X11
    #include "../platforms/linux/x11/hlnx_window.h"
    #include <vulkan/vulkan_xlib.h>
    #elif WAYLAND

    #endif

#endif

namespace hf
{
    const std::vector<const char*> REQUIRED_EXTENSIONS =
    {
#if PLATFORM_WINDOWS
        "VK_KHR_surface",
        "VK_KHR_win32_surface",
#elif PLATFORM_LINUX
        "VK_KHR_surface",
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

    void Graphics_LoadSurface(VKRendererData* rendererData)
    {

    }

    void Graphics_SetupPlatform()
    {

    }

#elif PLATFORM_LINUX

#if X11

    void Graphics_LoadSurface(VKRendererData* rendererData)
    {
        VkXlibSurfaceCreateInfoKHR createInfo
        {
            .sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
            .dpy = PLATFORM_DATA.display,
            .window = (::Window)rendererData->windowHandle,
        };

        VK_HANDLE_EXCEPT(vkCreateXlibSurfaceKHR(GRAPHICS_DATA.instance, &createInfo, nullptr, &rendererData->surface));
    }

    void Graphics_SetupPlatform()
    {

    }

#elif WAYLAND

    void Graphics_LoadSurface(VKRendererData* rendererData)
    {

    }

    void Graphics_SetupPlatform()
    {

    }

#endif

#endif

    void Graphics_UnloadSurface(VKRendererData* rendererData)
    {
        vkDestroySurfaceKHR(GRAPHICS_DATA.instance, rendererData->surface, nullptr);
        rendererData->surface = nullptr;
    }
}