#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    GraphicsData GRAPHICS_DATA;

    //Logical devices are destroyed in loader
    static void CreateLogicalDevice(GraphicsDevice& device);
    static bool SetupPhysicalDevice(VkSurfaceKHR surface, VkPhysicalDevice device, GraphicsDevice* deviceData);

    //------------------------------------------------------------------------------------

    VkRenderer::VkRenderer(const inter::rendering::RendererInstanceCreationInfo& info)
        : windowHandle(info.handle), targetSize(info.size), vSyncMode(info.vSyncMode)
    {
        if (!GRAPHICS_DATA.devicesAreLoaded) LoadDevice(windowHandle, &swapchain.surface);
        else VK_HANDLE_EXCEPT((VkResult)GRAPHICS_DATA.platform.createVulkanSurfaceFunc(windowHandle, GRAPHICS_DATA.instance, &swapchain.surface));
    }

    VkRenderer::~VkRenderer()
    {
        for (uint32_t i = 0; i < swapchain.images.size(); ++i)
        {
            auto& image = swapchain.images[i];
            DestroySemaphore(*GRAPHICS_DATA.defaultDevice, image.isRenderingFinished);
            DestroyFence(*GRAPHICS_DATA.defaultDevice, image.isInFlight);
        }

        for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; ++i)
            DestroySemaphore(*GRAPHICS_DATA.defaultDevice, frames[i].isImageAvailable);
        frames.clear();

        ClearRendererPassData(this);

        DestroySwapchainFrameBuffers(this);
        DestroySwapchain(swapchain, &swapchain.swapchain);
        DestroyCommandPool(*GRAPHICS_DATA.defaultDevice, commandPool);
        DestroySurface(this);
    }

    void PostRendererLoad(VkRenderer* rn, RenderPass mainPass)
    {
        rn->mainPass = mainPass;
        auto& pass = GetRenderPass(mainPass);
        if (!pass->hasPresentationAttachment) throw GENERIC_EXCEPT("[Vulkan]", "Main render pass must have presentation attachment");
        CreateSwapchain(rn->swapchain.surface, rn->targetSize, rn->vSyncMode,  &rn->swapchain);
        SetupViewportAndScissor(rn);

        CreateSwapchainFrameBuffers(rn);
        CreateCommandPool(*GRAPHICS_DATA.defaultDevice, GRAPHICS_DATA.defaultDevice->familyIndices.graphicsFamily.value(), &rn->commandPool);
        CreateCommandBuffers(*GRAPHICS_DATA.defaultDevice, &rn->commandPool, FRAMES_IN_FLIGHT);

        rn->frames = std::vector<VkFrame>(FRAMES_IN_FLIGHT);
        for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; ++i)
            CreateSemaphore(*GRAPHICS_DATA.defaultDevice, &rn->frames[i].isImageAvailable);

        for (auto& image : rn->swapchain.images)
        {
            CreateSemaphore(*GRAPHICS_DATA.defaultDevice, &image.isRenderingFinished);
            CreateFence(*GRAPHICS_DATA.defaultDevice, &image.isInFlight, true);
        }
    }

    //------------------------------------------------------------------------------------

    void CreateLogicalDevice(GraphicsDevice& device)
    {
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
        std::set uniqueQueueFamilies =
        {
            device.familyIndices.graphicsFamily.value(),
            device.familyIndices.presentFamily.value(),
            device.familyIndices.transferFamily.value()
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
            .enabledExtensionCount = GRAPHICS_DATA.platform.api->deviceExtensionCount,
            .ppEnabledExtensionNames = GRAPHICS_DATA.platform.api->deviceExtension,
            .pEnabledFeatures = &device.features,
        };

#if DEBUG
        createInfo.ppEnabledLayerNames = DEBUG_VALIDATION_LAYERS;
        createInfo.enabledLayerCount = NUM_VK_VALIDATION_LAYERS;
#endif

        VK_HANDLE_EXCEPT(vkCreateDevice(device.device, &createInfo, nullptr, &device.logicalDevice.device));

        auto& indices = device.familyIndices;
        vkGetDeviceQueue(device.logicalDevice.device, indices.graphicsFamily.value(),
            0, &device.logicalDevice.graphicsQueue);

        vkGetDeviceQueue(device.logicalDevice.device, indices.presentFamily.value(),
            0, &device.logicalDevice.presentQueue);

        vkGetDeviceQueue(device.logicalDevice.device, indices.transferFamily.value(),
            0, &device.logicalDevice.transferQueue);

        if (indices.graphicsFamily.value() == indices.computeFamily.value())
            device.logicalDevice.computeQueue = device.logicalDevice.graphicsQueue;
        else
        {
            vkGetDeviceQueue(device.logicalDevice.device, indices.computeFamily.value(),
            0, &device.logicalDevice.computeQueue);
        }
    }

    //------------------------------------------------------------------------------------

    bool SetupPhysicalDevice(VkSurfaceKHR surface, VkPhysicalDevice device, GraphicsDevice* deviceData)
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
            auto& flags = queueFamilies[i].queueFlags;
            if (flags & VK_QUEUE_GRAPHICS_BIT)
                deviceData->familyIndices.graphicsFamily = i;
            else if (flags & VK_QUEUE_TRANSFER_BIT)
                deviceData->familyIndices.transferFamily = i;
            else if (flags & VK_QUEUE_COMPUTE_BIT)
                deviceData->familyIndices.computeFamily = i;

            VkBool32 presentSupport = false;
            VK_HANDLE_EXCEPT(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport));

            if (presentSupport) deviceData->familyIndices.presentFamily = i;
            if (deviceData->familyIndices.IsComplete()) break;
        }

        auto& indices = deviceData->familyIndices;
        if (!indices.computeFamily.has_value() && indices.graphicsFamily.has_value())
        {
            auto& graphicsFlags = queueFamilies[indices.graphicsFamily.value()].queueFlags;
            if (graphicsFlags & VK_QUEUE_COMPUTE_BIT)
                deviceData->familyIndices.computeFamily = indices.graphicsFamily.value();
            else return false;
        }

        if (!indices.IsComplete()) return false;

        if (indices.graphicsFamily != indices.presentFamily)
        {
            deviceData->transferData =
            {
                .indices = { indices.graphicsFamily.value(), indices.presentFamily.value() },
                .sharingMode = VK_SHARING_MODE_CONCURRENT
            };
        }
        else deviceData->transferData =
        {
            .indices = {},
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE
        };

        auto& limits = deviceData->properties.limits;
        VkSampleCountFlags maxMsaa = limits.framebufferColorSampleCounts & limits.framebufferDepthSampleCounts;

        int32_t score = 0;
        score += (deviceData->properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) * 1000;
        score += (int32_t)deviceData->properties.limits.maxImageDimension2D;
        score += (int32_t)maxMsaa;
        deviceData->score = score;

        vkGetPhysicalDeviceMemoryProperties(deviceData->device, &deviceData->memProps);

        for (uint32_t i = 0; i < (uint32_t)TextureFormat::Count; ++i)
            vkGetPhysicalDeviceFormatProperties(deviceData->device, (VkFormat)i, &deviceData->formatProps[i]);
        return true;
    }

    void LoadDevice(void* windowHandle, VkSurfaceKHR* resultSurface)
    {
        uint32_t deviceCount = 0;
        VK_HANDLE_EXCEPT(vkEnumeratePhysicalDevices(GRAPHICS_DATA.instance,
            &deviceCount, nullptr));

        if (deviceCount == 0) throw GENERIC_EXCEPT("[Vulkan]", "No Graphics device found");

        std::vector<VkPhysicalDevice> availableDevices(deviceCount);
        VK_HANDLE_EXCEPT(vkEnumeratePhysicalDevices(GRAPHICS_DATA.instance,
                &deviceCount, availableDevices.data()));

        VK_HANDLE_EXCEPT((VkResult)GRAPHICS_DATA.platform.createVulkanSurfaceFunc(windowHandle, GRAPHICS_DATA.instance, resultSurface));

        for (const auto& device : availableDevices)
        {
            GraphicsDevice deviceData{};
            if (SetupPhysicalDevice(*resultSurface, device, &deviceData) &&
                CheckDeviceExtensionSupport(device))
            {
                SwapChainSupportDetails scs{};
                QuerySwapChainSupport(device, *resultSurface, &scs);

                if (!scs.formats.empty() &&
                    !scs.presentModes.empty())
                {
                    CreateLogicalDevice(deviceData);
                    GRAPHICS_DATA.suitableDevices.push_back(deviceData);
                }
            }
        }

        if (GRAPHICS_DATA.suitableDevices.empty())
            throw GENERIC_EXCEPT("[Vulkan]", "No suitable graphics device found");

        std::ranges::stable_sort(GRAPHICS_DATA.suitableDevices,
            [](const GraphicsDevice& a, const GraphicsDevice& b)
            { return a.score > b.score; });
        GRAPHICS_DATA.devicesAreLoaded = true;
        GRAPHICS_DATA.defaultDevice = &GRAPHICS_DATA.suitableDevices[0];
        LOG_LOG("Graphics device found [%s]", GRAPHICS_DATA.defaultDevice->properties.deviceName);

        VmaAllocatorCreateInfo allocatorInfo
        {
            .physicalDevice = GRAPHICS_DATA.defaultDevice->device,
            .device = GRAPHICS_DATA.defaultDevice->logicalDevice.device,
            .pAllocationCallbacks = nullptr,
            .instance = GRAPHICS_DATA.instance,
        };

        VK_HANDLE_EXCEPT(vmaCreateAllocator(&allocatorInfo, &GRAPHICS_DATA.allocator));

        CreateCommandPool(*GRAPHICS_DATA.defaultDevice, GRAPHICS_DATA.defaultDevice->familyIndices.transferFamily.value(), &GRAPHICS_DATA.transferPool);
        CreateCommandBuffers(*GRAPHICS_DATA.defaultDevice, &GRAPHICS_DATA.transferPool, 1);

        CreateCommandPool(*GRAPHICS_DATA.defaultDevice, GRAPHICS_DATA.defaultDevice->familyIndices.graphicsFamily.value(), &GRAPHICS_DATA.graphicsPool);
        CreateCommandBuffers(*GRAPHICS_DATA.defaultDevice, &GRAPHICS_DATA.graphicsPool, 1);
    }

    static void DestroyLogicalDevice(LogicalDevice& device)
    {
        if (device.device != VK_NULL_HANDLE)
        {
            vkDestroyDevice(device.device, nullptr);
            device.device = VK_NULL_HANDLE;
        }
    }

    void UnloadDevice()
    {
        vmaDestroyAllocator(GRAPHICS_DATA.allocator);
        DestroyCommandPool(*GRAPHICS_DATA.defaultDevice, GRAPHICS_DATA.transferPool);
        DestroyCommandPool(*GRAPHICS_DATA.defaultDevice, GRAPHICS_DATA.graphicsPool);

        for (auto& device : GRAPHICS_DATA.suitableDevices)
            DestroyLogicalDevice(device.logicalDevice);
    }
}