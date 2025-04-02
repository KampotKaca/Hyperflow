#include "hvk_graphics.h"

#include "hgenericexception.h"
#include "exceptions/hgraphicsexception.h"
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

    void InitLayers();
    void InitExtensions();
    void InitInstanceVersion();
    void InitDevice();
    bool SetupDevice(VkPhysicalDevice device, GraphicsDevice* deviceData);

    void CreateInstance(VkApplicationInfo& appInfo);
    void DestroyInstance();

    void Graphics_Load(const char* appVersion)
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

        CreateInstance(appInfo);

        InitDevice();
    }

    void Graphics_Unload()
    {
        DestroyInstance();
    }

    bool Graphics_IsLayerSupported(const char* layer)
    {
        for (const auto& availableLayer : GRAPHICS_DATA.availableLayers)
        {
            if (strcmp(layer, availableLayer.layerName) == 0) return true;
        }
        return false;
    }

    bool Graphics_IsExtensionSupported(const char* extension)
    {
        return GRAPHICS_DATA.availableExtensionNames.contains(extension);
    }

    void InitLayers()
    {
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        GRAPHICS_DATA.availableLayers = std::vector<VkLayerProperties>(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, GRAPHICS_DATA.availableLayers.data());
    }

    void InitExtensions()
    {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        GRAPHICS_DATA.availableExtensions = std::vector<VkExtensionProperties>(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, GRAPHICS_DATA.availableExtensions.data());

        for (const auto& ext : GRAPHICS_DATA.availableExtensions)
            GRAPHICS_DATA.availableExtensionNames.insert(ext.extensionName);

        for(auto& extension : REQUIRED_EXTENSIONS)
        {
            if (!Graphics_IsExtensionSupported(extension))
                throw GENERIC_EXCEPT("[Vulkan]", "[Required extension not supported]\n%s", extension);
        }
    }

    void InitInstanceVersion()
    {
        vkEnumerateInstanceVersion(&GRAPHICS_DATA.supportedVersion);
        LOG_LOG("Supported Vulkan API Version: (%i.%i.%i)",
            VK_VERSION_MAJOR(GRAPHICS_DATA.supportedVersion),
            VK_VERSION_MINOR(GRAPHICS_DATA.supportedVersion),
            VK_VERSION_PATCH(GRAPHICS_DATA.supportedVersion));
    }

    void InitDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(GRAPHICS_DATA.instance, &deviceCount, nullptr);

        if (deviceCount == 0) throw GENERIC_EXCEPT("[Vulkan]", "No Graphics device found");

        std::vector<VkPhysicalDevice> availableDevices(deviceCount);
        vkEnumeratePhysicalDevices(GRAPHICS_DATA.instance, &deviceCount, availableDevices.data());

        for (const auto& device : availableDevices)
        {
            GraphicsDevice deviceData{};
            if (SetupDevice(device, &deviceData)) GRAPHICS_DATA.suitableDevices.push_back(deviceData);
        }

        if (GRAPHICS_DATA.suitableDevices.size() == 0)
            throw GENERIC_EXCEPT("[Vulkan]", "No suitable graphics device found");

        std::stable_sort(GRAPHICS_DATA.suitableDevices.begin(), GRAPHICS_DATA.suitableDevices.end(),
                      [](const GraphicsDevice& a, const GraphicsDevice& b)
                      { return a.score > b.score; });

        GRAPHICS_DATA.defaultDevice = &GRAPHICS_DATA.suitableDevices[0];
        LOG_LOG("Graphics device found [%s]", GRAPHICS_DATA.defaultDevice->properties.deviceName);
    }

    bool SetupDevice(VkPhysicalDevice device, GraphicsDevice* deviceData)
    {
        vkGetPhysicalDeviceProperties(device, &deviceData->properties);
        vkGetPhysicalDeviceFeatures(device, &deviceData->features);

        if (!deviceData->features.geometryShader) return false;

        int32_t score = 0;
        score += (deviceData->properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) * 1000;
        score += deviceData->properties.limits.maxImageDimension2D;
        deviceData->score = score;
        return true;
    }

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
        {
            const char* validationLayers[] = { "VK_LAYER_KHRONOS_validation" };

            for (const char* layer : validationLayers)
            {
                if (!Graphics_IsLayerSupported(layer))
                    throw GENERIC_EXCEPT("[Vulkan Debug]", "Unsupported validation layer [%s]", layer);
            }

            createInfo.ppEnabledLayerNames = validationLayers;
            createInfo.enabledLayerCount = 1;
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &GRAPHICS_DATA.debugCreateInfo;
        }
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