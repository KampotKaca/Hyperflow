#ifndef HVK_GRAPHICS_H
#define HVK_GRAPHICS_H

#include "hshared.h"
#include <vulkan/vulkan.h>

namespace hf
{
#if DEBUG
    extern const std::vector<const char*> DEBUG_VALIDATION_LAYERS;

    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void* userData);
#endif

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
    };

    struct LogicalDevice
    {
        VkDevice device{};
        VkQueue queue{};
    };

    struct GraphicsDevice
    {
        VkPhysicalDevice device{};
        VkPhysicalDeviceProperties properties{};
        VkPhysicalDeviceFeatures features{};
        int32_t score{};

        QueueFamilyIndices familyIndices{};
        LogicalDevice logicalDevice{};
    };

    struct GraphicsData
    {
        int32_t rendererCount = 0;
        std::vector<VkLayerProperties> availableLayers{};
        std::vector<VkExtensionProperties> availableExtensions{};

        VkInstance instance{};
        uint32_t supportedVersion;
        std::set<std::string> availableExtensionNames{};

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

    struct VKRendererData
    {
        void* windowHandle = nullptr;
        VkSurfaceKHR surface{};
        std::vector<GraphicsDevice> suitableDevices{};
        GraphicsDevice* defaultDevice;
    };

    extern GraphicsData GRAPHICS_DATA;

    extern void Graphics_Load(const char* appVersion);
    extern void Graphics_Unload();
    extern bool Graphics_IsLayerSupported(const char* layer);
    extern bool Graphics_IsExtensionSupported(const char* extension);

    extern void Graphics_SetupPlatform();

    extern void Graphics_LoadSurface(VKRendererData* rendererData);
    extern void Graphics_UnloadSurface(VKRendererData* rendererData);

    extern void Graphics_LoadPhysicalDevices(VKRendererData* rendererData);
    extern void Graphics_UnloadPhysicalDevices(VKRendererData* rendererData);

    extern const std::vector<const char*> REQUIRED_EXTENSIONS;
}

#endif //HVK_GRAPHICS_H
