#include "hvk_graphics.h"

#include <bits/ranges_algo.h>

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
    bool CheckDeviceExtensionSupport(const VkPhysicalDevice& device);

    void QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, SwapChainSupportDetails* supportDetails);
    bool GetAvailableSurfaceDetails(const SwapChainSupportDetails& swapChainSupportDetails,
                                    VkFormat targetFormat, VkPresentModeKHR targetPresentMode, uvec2 targetExtents,
                                    GraphicsSwapchainDetails* result);

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
        VK_HANDLE_EXCEPT(vkEnumeratePhysicalDevices(GRAPHICS_DATA.instance,
            &deviceCount, nullptr));

        if (deviceCount == 0) throw GENERIC_EXCEPT("[Vulkan]", "No Graphics device found");

        std::vector<VkPhysicalDevice> availableDevices(deviceCount);
        VK_HANDLE_EXCEPT(vkEnumeratePhysicalDevices(GRAPHICS_DATA.instance,
            &deviceCount, availableDevices.data()));

        Graphics_LoadSurface(rendererData);

        for (const auto& device : availableDevices)
        {
            GraphicsDevice deviceData{};
            if (SetupPhysicalDevice(rendererData, device, &deviceData) &&
                CheckDeviceExtensionSupport(device))
            {
                QuerySwapChainSupport(device, rendererData->surface, &deviceData.swapChainSupport);

                if (!deviceData.swapChainSupport.formats.empty() &&
                    !deviceData.swapChainSupport.presentModes.empty())
                {
                    CreateLogicalDevice(deviceData);
                    rendererData->suitableDevices.push_back(deviceData);
                }
            }
        }

        if (rendererData->suitableDevices.empty())
            throw GENERIC_EXCEPT("[Vulkan]", "No suitable graphics device found");

        std::ranges::stable_sort(rendererData->suitableDevices,
                                 [](const GraphicsDevice& a, const GraphicsDevice& b)
                                 { return a.score > b.score; });
    }

    void Graphics_UnloadPhysicalDevices(VKRendererData* rendererData)
    {
        Graphics_UnloadSurface(rendererData);
        for (auto& device : rendererData->suitableDevices)
            DestroyLogicalDevice(device.logicalDevice);
    }

    uint32_t Graphics_GetDeviceCount(const VKRendererData* rendererData) { return rendererData->suitableDevices.size(); }

    void Graphics_LoadSwapchain(VKRendererData* rendererData, uint32_t deviceIndex)
    {
        if (deviceIndex >= rendererData->suitableDevices.size())
            throw GENERIC_EXCEPT("[Vulkan]", "Invalid device index [%d]", deviceIndex);

        Graphics_UnloadSwapchain(rendererData);
        rendererData->defaultDevice = &rendererData->suitableDevices[deviceIndex];
        LOG_LOG("Graphics device found [%s]", rendererData->defaultDevice->properties.deviceName);

        auto& scs = rendererData->defaultDevice->swapChainSupport;
        GraphicsSwapchainDetails details{};
        if (GetAvailableSurfaceDetails(scs,
            VULKAN_API_COLOR_FORMAT, VULKAN_API_PRESENT_MODE, ivec2(1080, 1920), &details))
        {
            uint32_t imageCount = scs.capabilities.minImageCount + 1;
            uint32_t maxImageCount = scs.capabilities.maxImageCount;
            if (maxImageCount > 0 && imageCount > maxImageCount) imageCount = maxImageCount;

            VkSwapchainCreateInfoKHR createInfo
            {
                .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .surface = rendererData->surface,
                .minImageCount = imageCount,
                .imageFormat = details.format.format,
                .imageColorSpace = details.format.colorSpace,
                .imageExtent = details.extent,
                .imageArrayLayers = 1,
                .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                .preTransform = scs.capabilities.currentTransform,
                .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                .presentMode = details.presentMode,
                .clipped = VK_TRUE,
                .oldSwapchain = VK_NULL_HANDLE
            };

            auto& indices = rendererData->defaultDevice->familyIndices;
            if (indices.graphicsFamily != indices.presentFamily)
            {
                uint32_t familyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
                createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                createInfo.queueFamilyIndexCount = 2;
                createInfo.pQueueFamilyIndices = familyIndices;
            }
            else
            {
                createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                createInfo.queueFamilyIndexCount = 0;
                createInfo.pQueueFamilyIndices = nullptr;
            }

            VK_HANDLE_EXCEPT(vkCreateSwapchainKHR(rendererData->defaultDevice->logicalDevice.device, &createInfo,
                nullptr, &rendererData->swapchain.swapchain));
            rendererData->swapchain.details = details;
        }
        else throw GENERIC_EXCEPT("[Vulkan]", "Unable to create swapchain");

        uint32_t imageCount;
        VK_HANDLE_EXCEPT(vkGetSwapchainImagesKHR(rendererData->defaultDevice->logicalDevice.device,
            rendererData->swapchain.swapchain, &imageCount, nullptr));

        rendererData->swapchain.swapchainImages = std::vector<VkImage>(imageCount);
        VK_HANDLE_EXCEPT(vkGetSwapchainImagesKHR(rendererData->defaultDevice->logicalDevice.device,
            rendererData->swapchain.swapchain, &imageCount,
            rendererData->swapchain.swapchainImages.data()));
    }

    void Graphics_UnloadSwapchain(VKRendererData* rendererData)
    {
        if (rendererData->defaultDevice != nullptr)
        {
            vkDestroySwapchainKHR(rendererData->defaultDevice->logicalDevice.device,
                rendererData->swapchain.swapchain, nullptr);
            rendererData->swapchain.swapchain = VK_NULL_HANDLE;
            rendererData->swapchain.swapchainImages.clear();
        }
    }

    //------------------------------------------------------------------------------------

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
            if (!Graphics_IsExtensionSupported(extension))
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
            .queueCreateInfoCount = (uint32_t)queueCreateInfos.size(),
            .pQueueCreateInfos = queueCreateInfos.data(),
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

    void QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, SwapChainSupportDetails* supportDetails)
    {
        VK_HANDLE_EXCEPT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &supportDetails->capabilities));

        uint32_t formatCount;
        VK_HANDLE_EXCEPT(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr));
        if (formatCount > 0)
        {
            supportDetails->formats = std::vector<VkSurfaceFormatKHR>(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                supportDetails->formats.data());
        }

        uint32_t presentModeCount;
        VK_HANDLE_EXCEPT(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr));

        if (presentModeCount > 0)
        {
            supportDetails->presentModes = std::vector<VkPresentModeKHR>(presentModeCount);
            VK_HANDLE_EXCEPT(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount,
                supportDetails->presentModes.data()));
        }
    }

    bool CheckDeviceExtensionSupport(const VkPhysicalDevice& device)
    {
        uint32_t extensionCount;
        VK_HANDLE_EXCEPT(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr));

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        VK_HANDLE_EXCEPT(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data()));

        std::set<std::string> requiredExtensions(DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end());

        for (const auto& extension : availableExtensions)
            requiredExtensions.erase(extension.extensionName);

        return requiredExtensions.empty();
    }

    bool GetAvailableSurfaceDetails(const SwapChainSupportDetails& swapChainSupportDetails,
    VkFormat targetFormat, VkPresentModeKHR targetPresentMode, uvec2 targetExtents,
    GraphicsSwapchainDetails* result)
    {
        int mask = 0;
        for (auto& format : swapChainSupportDetails.formats)
        {
            if (format.format == targetFormat)
            {
                result->format = format;
                mask |= 1 << 0;
                break;
            }
        }

        for (auto& presentMode : swapChainSupportDetails.presentModes)
        {
            if (presentMode == targetPresentMode)
            {
                result->presentMode = presentMode;
                mask |= 1 << 1;
                break;
            }
        }

        VkExtent2D extents = { targetExtents.x, targetExtents.y };
        extents.width = std::clamp(extents.width, swapChainSupportDetails.capabilities.minImageExtent.width,
            swapChainSupportDetails.capabilities.maxImageExtent.width);
        extents.height = std::clamp(extents.height, swapChainSupportDetails.capabilities.minImageExtent.height,
            swapChainSupportDetails.capabilities.maxImageExtent.height);

        result->extent = extents;
        if (!(mask & (1 << 0))) LOG_WARN("[Vulkan] %s", "Unable to choose target swapchain surface format");
        if (!(mask & (1 << 1))) LOG_WARN("[Vulkan] %s", "Unable to choose target swapchain present mode");

        return mask == (1 << 2) - 1;
    }

    bool QueueFamilyIndices::IsComplete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }
}