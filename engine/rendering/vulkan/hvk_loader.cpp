#include "hvk_graphics.h"
#include "hinternal.h"
#include "hshared.h"
#include "exceptions/hgraphicsexception.h"
#include "hyperflow.h"
#include "../config.h"
#include "hwindow.h"

namespace hf::inter::rendering
{
    const std::vector<const char*> DEVICE_EXTENSIONS
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

#if DEBUG
    const std::vector<const char*> DEBUG_VALIDATION_LAYERS =
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

    void* CreateInstance(const Window* window)
    {
        auto rendererData = new VKRendererData();
        if (window) rendererData->windowHandle = window->handle;
        CreateSurface(rendererData);
        SetupViewportAndScissor(rendererData);
        return rendererData;
    }

    void DestroyInstance(void* rnInstance)
    {
        const auto data = (VKRendererData*)rnInstance;
        DestroySurface(data);
        delete(data);
    }

    static void InitLayers();
    static void InitExtensions();
    static void InitInstanceVersion();

    static void CreateInstance(VkApplicationInfo& appInfo);
    static void DestroyInstance();

    static void DestroyLogicalDevice(LogicalDevice& device);

    void Load(const char* appVersion)
    {
        auto engineV = utils::ConvertVersion(VERSION);
        auto appV = utils::ConvertVersion(appVersion);
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = GetApplicationTitle().c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(appV.x, appV.y, appV.z);
        appInfo.pEngineName = HF_ENGINE_TITLE;
        appInfo.engineVersion = VK_MAKE_VERSION(engineV.x, engineV.y, engineV.z);
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

    void Unload()
    {
        for (auto& device : GRAPHICS_DATA.suitableDevices)
            DestroyLogicalDevice(device.logicalDevice);

        DestroyInstance();
    }

    //--------------------------------------------------------------------------

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

        for(auto& extension : REQUIRED_EXTENSIONS)
        {
            if (!IsExtensionSupported(extension))
                throw GENERIC_EXCEPT("[Vulkan]", "[Required extension not supported]\n%s", extension);
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
            .enabledExtensionCount = (uint32_t)REQUIRED_EXTENSIONS.size(),
            .ppEnabledExtensionNames = REQUIRED_EXTENSIONS.data(),
        };

#if DEBUG
        createInfo.ppEnabledLayerNames = DEBUG_VALIDATION_LAYERS.data();
        createInfo.enabledLayerCount = DEBUG_VALIDATION_LAYERS.size();
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

    void DestroyLogicalDevice(LogicalDevice& device)
    {
        vkDestroyDevice(device.device, nullptr);
    }
}