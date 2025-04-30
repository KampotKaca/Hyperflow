#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
#if DEBUG

    const char* DEBUG_VALIDATION_LAYERS[NUM_VK_VALIDATION_LAYERS] =
    {
        "VK_LAYER_KHRONOS_validation",
    };

    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void* userData)
    {
        switch (severity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            LOG_LOG("[Vulkan Debug Callback] %s", callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LOG_WARN("[Vulkan Debug Callback] %s", callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LOG_ERROR("[Vulkan Debug Callback] %s", callbackData->pMessage);
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

    static void InitLayers();
    static void InitExtensions();
    static void InitInstanceVersion();

    static void CreateInstance(VkApplicationInfo& appInfo);
    static void DestroyInstance();

    void LoadVulkan(const inter::rendering::RendererLoadInfo& info)
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = info.applicationTitle;
        appInfo.applicationVersion = VK_MAKE_VERSION(info.appVersion.x, info.appVersion.y, info.appVersion.z);
        appInfo.pEngineName = HF_ENGINE_TITLE;
        appInfo.engineVersion = VK_MAKE_VERSION(info.engineVersion.x, info.engineVersion.y, info.engineVersion.z);
        appInfo.apiVersion = VULKAN_API_VERSION;

        InitLayers();
        InitExtensions();
        InitInstanceVersion();

#if DEBUG
        for (const char* layer : DEBUG_VALIDATION_LAYERS)
        {
            if (!IsLayerSupported(layer))
                throw GENERIC_EXCEPT("[Vulkan Debug]", "Unsupported validation layer [%s]", layer);
        }
#endif

        CreateInstance(appInfo);
    }

    void UnloadVulkan()
    {
        SubmitStagedCopyOperations();
        DelayThreadUntilRenderingFinish();
        UnloadDevice();
        DestroyInstance();
    }

    //--------------------------------------------------------------------------

    void DelayThreadUntilRenderingFinish()
    {
        VK_HANDLE_EXCEPT(vkDeviceWaitIdle(GRAPHICS_DATA.defaultDevice->logicalDevice.device));
    }

    void InitLayers()
    {
        uint32_t layerCount = 0;
        VK_HANDLE_EXCEPT(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));
        GRAPHICS_DATA.availableLayers = std::vector<VkLayerProperties>(layerCount);
        VK_HANDLE_EXCEPT(vkEnumerateInstanceLayerProperties(&layerCount, GRAPHICS_DATA.availableLayers.data()));
    }

    void InitExtensions()
    {
        uint32_t extensionCount = 0;
        VK_HANDLE_EXCEPT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr));
        GRAPHICS_DATA.availableExtensions = std::vector<VkExtensionProperties>(extensionCount);
        VK_HANDLE_EXCEPT(vkEnumerateInstanceExtensionProperties(nullptr,
            &extensionCount, GRAPHICS_DATA.availableExtensions.data()));

        for (const auto& ext : GRAPHICS_DATA.availableExtensions)
            GRAPHICS_DATA.availableExtensionNames.insert(ext.extensionName);

        for (uint32_t i = 0; i < GRAPHICS_DATA.platform.api->requiredExtensionCount; ++i)
        {
            auto& ext = GRAPHICS_DATA.platform.api->requiredExtension[i];
            if (!IsExtensionSupported(ext))
                throw GENERIC_EXCEPT("[Vulkan]", "[Required extension not supported]\n%s", ext);
        }
    }

    void InitInstanceVersion()
    {
        VK_HANDLE_EXCEPT(vkEnumerateInstanceVersion(&GRAPHICS_DATA.supportedVersion));
        LOG_LOG("Supported Vulkan API Version: (%i.%i.%i)",
            VK_VERSION_MAJOR(GRAPHICS_DATA.supportedVersion),
            VK_VERSION_MINOR(GRAPHICS_DATA.supportedVersion),
            VK_VERSION_PATCH(GRAPHICS_DATA.supportedVersion));
    }

    //--------------------------------------------------------------------------

    void CreateInstance(VkApplicationInfo& appInfo)
    {
        VkInstanceCreateInfo createInfo
        {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &appInfo,
            .enabledLayerCount = 0,
            .enabledExtensionCount = GRAPHICS_DATA.platform.api->requiredExtensionCount,
            .ppEnabledExtensionNames = GRAPHICS_DATA.platform.api->requiredExtension,
        };

#if DEBUG
        createInfo.ppEnabledLayerNames = DEBUG_VALIDATION_LAYERS;
        createInfo.enabledLayerCount = NUM_VK_VALIDATION_LAYERS;
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&GRAPHICS_DATA.debugCreateInfo;
#endif

        VK_HANDLE_EXCEPT(vkCreateInstance(&createInfo, nullptr, &GRAPHICS_DATA.instance));

#if DEBUG
        VK_HANDLE_EXCEPT(Debug_CreateUtilsMessengerEXT(GRAPHICS_DATA.instance, &GRAPHICS_DATA.debugCreateInfo,
                            nullptr, &GRAPHICS_DATA.debugMessenger));
#endif
    }

    void DestroyInstance()
    {
#if DEBUG
        Debug_DestroyUtilsMessengerEXT(GRAPHICS_DATA.instance, GRAPHICS_DATA.debugMessenger, nullptr);
#endif

        vkDestroyInstance(GRAPHICS_DATA.instance, nullptr);
    }
}