#include "hvk_graphics.h"

#include "hunsupportedexception.h"
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

#if DEBUG
    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void* userData)
    {
        switch (severity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            LOG_LOG("[Vulkan Debug Callback]\n%s", callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LOG_WARN("[Vulkan Debug Callback]\n%s", callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LOG_ERROR("[Vulkan Debug Callback]\n%s", callbackData->pMessage);
            break;
        default: break;
        }
        return VK_FALSE;
    }

    VkResult Debug_CreateUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    void Debug_DestroyUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) func(instance, debugMessenger, pAllocator);
    }
#endif

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

        vkEnumerateInstanceVersion(&GRAPHICS_DATA.supportedVersion);
        LOG_LOG("Supported Vulkan API Version: (%i.%i.%i)",
            VK_VERSION_MAJOR(GRAPHICS_DATA.supportedVersion),
            VK_VERSION_MINOR(GRAPHICS_DATA.supportedVersion),
            VK_VERSION_PATCH(GRAPHICS_DATA.supportedVersion));
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