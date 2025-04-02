#include "hvk_graphics.h"

#include <hunsupportedexception.h>
#include <set>

#include "hyperflow.h"
#include "../config.h"

namespace hf
{
    GraphicsData GRAPHICS_DATA;
    const std::vector<const char*> REQUIRED_EXTENSIONS =
    {
#if PLATFORM_WINDOWS
        "VK_KHR_surface",
        "VK_KHR_win32_surface",
#elif PLATFORM_LINUX
        "VK_KHR_surface",
    #if X11
        "VK_KHR_xlib_surface",
    #elif WAILAND
        "VK_KHR_wayland_surface",
    #endif
#endif

#if DEBUG
        "VK_EXT_debug_utils"
#endif
    };

    void GraphicsLoad(const char* appVersion)
    {
        auto engineV = utils::ConvertVersion(VERSION);
        auto appV = utils::ConvertVersion(appVersion);
        auto& appInfo = GRAPHICS_DATA.appInfo;
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = GetApplicationTitle().c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(appV.x, appV.y, appV.z);
        appInfo.pEngineName = HF_ENGINE_TITLE;
        appInfo.engineVersion = VK_MAKE_VERSION(engineV.x, engineV.y, engineV.z);
        appInfo.apiVersion = VULKAN_API_VERSION;

        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        GRAPHICS_DATA.availableLayers = std::vector<VkLayerProperties>(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, GRAPHICS_DATA.availableLayers.data());

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        GRAPHICS_DATA.availableExtensions = std::vector<VkExtensionProperties>(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, GRAPHICS_DATA.availableExtensions.data());

        for (const auto& ext : GRAPHICS_DATA.availableExtensions)
            GRAPHICS_DATA.availableExtensionNames.insert(ext.extensionName);

        for(auto& extension : REQUIRED_EXTENSIONS)
        {
            if (!GraphicsValidateExtensionSupport(extension))
                throw UNSUPPORTED_EXCEPT_EXT("[Required Vulkan extension not supported]\n%s", extension);
        }
    }

    void GraphicsUnload()
    {

    }

    bool GraphicsValidateLayerSupport(const char* layer)
    {
        for (const auto& availableLayer : GRAPHICS_DATA.availableLayers)
        {
            if (strcmp(layer, availableLayer.layerName) == 0) return true;
        }
        return false;
    }

    bool GraphicsValidateExtensionSupport(const char* extension)
    {
        return GRAPHICS_DATA.availableExtensionNames.contains(extension);
    }

}