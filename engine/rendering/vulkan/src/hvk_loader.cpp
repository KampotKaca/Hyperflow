#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
#if defined(VK_ENABLE_VALIDATION)

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

    static void CreateInstance(const VkApplicationInfo& appInfo);
    static void DestroyInstance();

    static void* VulkanAllocationCallback(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope scope)
    { return GRAPHICS_DATA.platform.functions.allocateAlignedFunc(size, alignment); }

    static void VulkanDeallocationCallback(void* pUserData, void* pMemory) { GRAPHICS_DATA.platform.functions.deallocateFunc(pMemory); }

    static void* VulkanReallocationCallback(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
    { return GRAPHICS_DATA.platform.functions.reallocateFunc(pOriginal, size); }

    void LoadVulkan(const inter::rendering::RendererLoadInfo_i& info)
    {
        GRAPHICS_DATA.platform.allocator.pUserData = nullptr;
        GRAPHICS_DATA.platform.allocator.pfnAllocation = VulkanAllocationCallback;
        GRAPHICS_DATA.platform.allocator.pfnFree = VulkanDeallocationCallback;
        GRAPHICS_DATA.platform.allocator.pfnReallocation = VulkanReallocationCallback;

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

#if defined(VK_ENABLE_VALIDATION)
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
        SubmitAllOperations();

        GRAPHICS_DATA.shaderLayouts.clear();
        GRAPHICS_DATA.bufferAttribs.clear();
        GRAPHICS_DATA.boundBuffers.clear();
        GRAPHICS_DATA.textureSamplers.clear();
        GRAPHICS_DATA.textureLayouts.clear();

        UnloadEditorInfo();
        UnloadDevice();
        DestroyInstance();
    }

    //--------------------------------------------------------------------------

    void WaitForDevice()
    {
        VK_HANDLE_EXCEPT(vkDeviceWaitIdle(GRAPHICS_DATA.device.logicalDevice.device));
    }

    void InitLayers()
    {
        uint32_t layerCount = 0;
        VK_HANDLE_EXCEPT(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));
        GRAPHICS_DATA.availableLayers = List<VkLayerProperties>(layerCount);
        VK_HANDLE_EXCEPT(vkEnumerateInstanceLayerProperties(&layerCount, GRAPHICS_DATA.availableLayers.data()));
    }

    void InitExtensions()
    {
        uint32_t extensionCount = 0;
        VK_HANDLE_EXCEPT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr));
        GRAPHICS_DATA.availableExtensions = List<VkExtensionProperties>(extensionCount);
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
        LOG_LOG("[Vulkan] Supported Vulkan API Version: (%i.%i.%i)",
            VK_VERSION_MAJOR(GRAPHICS_DATA.supportedVersion),
            VK_VERSION_MINOR(GRAPHICS_DATA.supportedVersion),
            VK_VERSION_PATCH(GRAPHICS_DATA.supportedVersion));
    }

    //--------------------------------------------------------------------------

    void CreateInstance(const VkApplicationInfo& appInfo)
    {
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = 0;
        createInfo.enabledExtensionCount = GRAPHICS_DATA.platform.api->requiredExtensionCount;
        createInfo.ppEnabledExtensionNames = GRAPHICS_DATA.platform.api->requiredExtension;

#if defined(VK_ENABLE_VALIDATION)
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

#if defined(VK_ENABLE_DEEP_VALIDATION)

        constexpr VkValidationFeatureEnableEXT enables[] =
        {
            VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
            VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT,
            VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT
        };

        constexpr VkValidationFeatureDisableEXT disables[] =
        {
            VK_VALIDATION_FEATURE_DISABLE_CORE_CHECKS_EXT
        };

        VkValidationFeaturesEXT features{};
        features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        features.enabledValidationFeatureCount = 3;
        features.pEnabledValidationFeatures = enables;
        features.disabledValidationFeatureCount = 1;
        features.pDisabledValidationFeatures = disables;
        debugCreateInfo.pNext = &features;

#endif

        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = DebugCallback;

        createInfo.ppEnabledLayerNames = DEBUG_VALIDATION_LAYERS;
        createInfo.enabledLayerCount = NUM_VK_VALIDATION_LAYERS;
        createInfo.pNext = &debugCreateInfo;
#endif
        VK_HANDLE_EXCEPT(vkCreateInstance(&createInfo, &GRAPHICS_DATA.platform.allocator, &GRAPHICS_DATA.instance));

#if defined(VK_ENABLE_VALIDATION)
        VK_HANDLE_EXCEPT(Debug_CreateUtilsMessengerEXT(GRAPHICS_DATA.instance, &debugCreateInfo,
                            &GRAPHICS_DATA.platform.allocator, &GRAPHICS_DATA.debugMessenger));
#endif
    }

    void DestroyInstance()
    {
#if defined(VK_ENABLE_VALIDATION)
        Debug_DestroyUtilsMessengerEXT(GRAPHICS_DATA.instance, GRAPHICS_DATA.debugMessenger, &GRAPHICS_DATA.platform.allocator);
#endif

        vkDestroyInstance(GRAPHICS_DATA.instance, &GRAPHICS_DATA.platform.allocator);
    }

    void* LoadEditorInfo()
    {
        GRAPHICS_DATA.editorInfo = new RenderApiEditorInfo();

        const VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000 * (int32_t)std::size(pool_sizes);
        pool_info.poolSizeCount = (uint32_t)std::size(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;

        VK_HANDLE_EXCEPT(vkCreateDescriptorPool(GRAPHICS_DATA.device.logicalDevice.device,
            &pool_info, &GRAPHICS_DATA.platform.allocator, &GRAPHICS_DATA.editorInfo->descriptorPool));

        GRAPHICS_DATA.editorInfo->version = GRAPHICS_DATA.supportedVersion;
        GRAPHICS_DATA.editorInfo->instance = GRAPHICS_DATA.instance;
        GRAPHICS_DATA.editorInfo->physicalDevice = GRAPHICS_DATA.device.device;
        GRAPHICS_DATA.editorInfo->device = GRAPHICS_DATA.device.logicalDevice.device;
        GRAPHICS_DATA.editorInfo->queueFamily = GRAPHICS_DATA.device.familyIndices.graphicsFamily.value();
        GRAPHICS_DATA.editorInfo->queue = GRAPHICS_DATA.device.logicalDevice.graphicsQueue;

        GRAPHICS_DATA.editorInfo->CheckVkResultFn = [](VkResult err)
        {
            VK_HANDLE_EXCEPT(err);
        };

        return GRAPHICS_DATA.editorInfo;
    }

    void UnloadEditorInfo()
    {
        if (GRAPHICS_DATA.editorInfo)
        {
            vkDestroyDescriptorPool(GRAPHICS_DATA.device.logicalDevice.device,
                GRAPHICS_DATA.editorInfo->descriptorPool, &GRAPHICS_DATA.platform.allocator);
            delete GRAPHICS_DATA.editorInfo;
            GRAPHICS_DATA.editorInfo = nullptr;
        }
    }
}