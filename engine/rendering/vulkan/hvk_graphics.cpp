#include "hvk_graphics.h"

#include "hgenericexception.h"
#include "exceptions/hgraphicsexception.h"
#include "hyperflow.h"
#include "../config.h"

namespace hf
{
    GraphicsData GRAPHICS_DATA;
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

    void InitLayers();
    void InitExtensions();
    void InitInstanceVersion();

    void CreateInstance(VkApplicationInfo& appInfo);
    void DestroyInstance();

    void CreateLogicalDevice(GraphicsDevice& device);
    void DestroyLogicalDevice(LogicalDevice& device);

    bool SetupPhysicalDevice(const VKRendererData* renderer, VkPhysicalDevice device, GraphicsDevice* deviceData);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

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

#if DEBUG
        for (const char* layer : DEBUG_VALIDATION_LAYERS)
        {
            if (!Graphics_IsLayerSupported(layer))
                throw GENERIC_EXCEPT("[Vulkan Debug]", "Unsupported validation layer [%s]", layer);
        }
#endif

        CreateInstance(appInfo);
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

    //------------------------------------------------------------------------------------

    void Graphics_LoadPhysicalDevices(VKRendererData* rendererData)
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(GRAPHICS_DATA.instance, &deviceCount, nullptr);

        if (deviceCount == 0) throw GENERIC_EXCEPT("[Vulkan]", "No Graphics device found");

        std::vector<VkPhysicalDevice> availableDevices(deviceCount);
        vkEnumeratePhysicalDevices(GRAPHICS_DATA.instance, &deviceCount, availableDevices.data());

        Graphics_LoadSurface(rendererData);

        for (const auto& device : availableDevices)
        {
            GraphicsDevice deviceData{};
            if (SetupPhysicalDevice(rendererData, device, &deviceData) &&
                CheckDeviceExtensionSupport(device))
            {
                CreateLogicalDevice(deviceData);
                rendererData->suitableDevices.push_back(deviceData);
            }
        }

        if (rendererData->suitableDevices.empty())
            throw GENERIC_EXCEPT("[Vulkan]", "No suitable graphics device found");

        std::stable_sort(rendererData->suitableDevices.begin(), rendererData->suitableDevices.end(),
                      [](const GraphicsDevice& a, const GraphicsDevice& b)
                      { return a.score > b.score; });

        rendererData->defaultDevice = &rendererData->suitableDevices[0];
        LOG_LOG("Graphics device found [%s]", rendererData->defaultDevice->properties.deviceName);
    }

    extern void Graphics_UnloadPhysicalDevices(VKRendererData* rendererData)
    {
        Graphics_UnloadSurface(rendererData);
        for (auto& device : rendererData->suitableDevices)
            DestroyLogicalDevice(device.logicalDevice);
    }

    //------------------------------------------------------------------------------------

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

    //------------------------------------------------------------------------------------

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

    //------------------------------------------------------------------------------------

    void CreateLogicalDevice(GraphicsDevice& device)
    {
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
        std::set<uint32_t> uniqueQueueFamilies =
        {
            device.familyIndices.graphicsFamily.value(),
            device.familyIndices.presentFamily.value()
        };

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkDeviceCreateInfo createInfo
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pQueueCreateInfos = queueCreateInfos.data(),
            .queueCreateInfoCount = (uint32_t)queueCreateInfos.size(),
            .enabledLayerCount = 0,
            .enabledExtensionCount = (uint32_t)DEVICE_EXTENSIONS.size(),
            .ppEnabledExtensionNames = DEVICE_EXTENSIONS.data(),
            .pEnabledFeatures = &device.features,
        };

#if DEBUG
        createInfo.ppEnabledLayerNames = DEBUG_VALIDATION_LAYERS.data();
        createInfo.enabledLayerCount = DEBUG_VALIDATION_LAYERS.size();
#endif

        VK_HANDLE_EXCEPT(vkCreateDevice(device.device, &createInfo, nullptr, &device.logicalDevice.device));

        vkGetDeviceQueue(device.logicalDevice.device, device.familyIndices.graphicsFamily.value(),
            0, &device.logicalDevice.graphicsQueue);

        vkGetDeviceQueue(device.logicalDevice.device, device.familyIndices.presentFamily.value(),
            0, &device.logicalDevice.presentQueue);
    }

    void DestroyLogicalDevice(LogicalDevice& device)
    {
        vkDestroyDevice(device.device, nullptr);
    }

    //------------------------------------------------------------------------------------

    bool SetupPhysicalDevice(const VKRendererData* renderer, VkPhysicalDevice device, GraphicsDevice* deviceData)
    {
        deviceData->device = device;
        vkGetPhysicalDeviceProperties(device, &deviceData->properties);
        vkGetPhysicalDeviceFeatures(device, &deviceData->features);

        if (!deviceData->features.geometryShader) return false;

        uint32_t familyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(familyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilies.size(); i++)
        {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                deviceData->familyIndices.graphicsFamily = i;

            VkBool32 presentSupport = false;
            VK_HANDLE_EXCEPT(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, renderer->surface, &presentSupport));

            if (presentSupport) deviceData->familyIndices.presentFamily = i;
            if (deviceData->familyIndices.IsComplete()) break;
        }

        if (!deviceData->familyIndices.IsComplete()) return false;

        int32_t score = 0;
        score += (deviceData->properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) * 1000;
        score += (int32_t)deviceData->properties.limits.maxImageDimension2D;
        deviceData->score = score;

        return true;
    }

    bool CheckDeviceExtensionSupport(const VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end());

        for (const auto& extension : availableExtensions)
            requiredExtensions.erase(extension.extensionName);

        return requiredExtensions.empty();
    }

    bool QueueFamilyIndices::IsComplete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }
}