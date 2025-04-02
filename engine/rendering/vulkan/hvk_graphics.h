#ifndef HVK_GRAPHICS_H
#define HVK_GRAPHICS_H

#include "hshared.h"
#include <vulkan/vulkan.h>

namespace hf
{
#if DEBUG
    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void* userData);

    VkResult Debug_CreateUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void Debug_DestroyUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
#endif

    struct GraphicsData
    {
        int32_t rendererCount = 0;
        std::vector<VkLayerProperties> availableLayers{};
        std::vector<VkExtensionProperties> availableExtensions{};
        std::set<std::string> availableExtensionNames{};
        VkApplicationInfo appInfo{};
        uint32_t supportedVersion;
    };

    struct VKRendererData
    {
        void* windowHandle = nullptr;
        VkInstance instance{};

#if DEBUG
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = DebugCallback
        };
        VkDebugUtilsMessengerEXT debugMessenger{};
#endif
    };

    extern GraphicsData GRAPHICS_DATA;
    extern void GraphicsLoad(const char* appVersion);
    extern void GraphicsUnload();
    extern bool GraphicsValidateLayerSupport(const char* layer);
    extern bool GraphicsValidateExtensionSupport(const char* extension);

    extern const std::vector<const char*> REQUIRED_EXTENSIONS;
}

#endif //HVK_GRAPHICS_H
