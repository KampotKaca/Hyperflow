#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    GraphicsData GRAPHICS_DATA;

    //Logical devices are destroyed in loader
    static void CreateLogicalDevice(GraphicsDevice& device);
    static bool SetupPhysicalDevice(const VKRenderer* renderer, VkPhysicalDevice device, GraphicsDevice* deviceData);

    //------------------------------------------------------------------------------------

    VKRenderer::VKRenderer(void* handle, uvec2 size) : windowHandle(handle), targetSize(size)
    {
        if (!GRAPHICS_DATA.devicesAreLoaded)
        {
            uint32_t deviceCount = 0;
            VK_HANDLE_EXCEPT(vkEnumeratePhysicalDevices(GRAPHICS_DATA.instance,
                &deviceCount, nullptr));

            if (deviceCount == 0) throw GENERIC_EXCEPT("[Vulkan]", "No Graphics device found");

            std::vector<VkPhysicalDevice> availableDevices(deviceCount);
            VK_HANDLE_EXCEPT(vkEnumeratePhysicalDevices(GRAPHICS_DATA.instance,
                &deviceCount, availableDevices.data()));

            GRAPHICS_DATA.platform.api->CreateSurface(GRAPHICS_DATA.platform.instance, windowHandle, GRAPHICS_DATA.instance, &swapchain.surface);

            for (const auto& device : availableDevices)
            {
                GraphicsDevice deviceData{};
                if (SetupPhysicalDevice(this, device, &deviceData) &&
                    CheckDeviceExtensionSupport(device))
                {
                    SwapChainSupportDetails scs{};
                    QuerySwapChainSupport(device, swapchain.surface, &scs);

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

            CreateSwapchain(swapchain.surface, targetSize, &swapchain);
            SetupViewportAndScissor(this);

            VkRenderPassAttachmentType attachments[] =
            {
                VkRenderPassAttachmentType::Color,
            };
            VkRenderPassCreationInfo renderPassInfo
            {
                .pAttachments = attachments,
                .attachmentCount = 1,
            };
            CreateRenderPass(renderPassInfo, &GRAPHICS_DATA.renderPass);

            VkPipelineLayoutCreationInfo pipelineLayoutInfo
            {
                .layoutCount = 0
            };
            CreatePipelineLayout(pipelineLayoutInfo, &GRAPHICS_DATA.pipelineLayout);
            CreateCommandPool(*GRAPHICS_DATA.defaultDevice, GRAPHICS_DATA.defaultDevice->familyIndices.transferFamily.value(), &GRAPHICS_DATA.transferPool);
            CreateCommandBuffers(*GRAPHICS_DATA.defaultDevice, &GRAPHICS_DATA.transferPool, 1);
        }
        else
        {
            GRAPHICS_DATA.platform.api->CreateSurface(GRAPHICS_DATA.platform.instance, windowHandle, GRAPHICS_DATA.instance, &swapchain.surface);
            CreateSwapchain(swapchain.surface, targetSize, &swapchain);
            SetupViewportAndScissor(this);
        }

        CreateRendererFrameBuffers(this);
        CreateCommandPool(*GRAPHICS_DATA.defaultDevice, GRAPHICS_DATA.defaultDevice->familyIndices.graphicsFamily.value(), &commandPool);
        CreateCommandBuffers(*GRAPHICS_DATA.defaultDevice, &commandPool, FRAMES_IN_FLIGHT);

        frames = std::vector<VkFrame>(FRAMES_IN_FLIGHT);
        for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; ++i) CreateFrame(&frames[i]);
    }

    VKRenderer::~VKRenderer()
    {
        for (auto& frame : frames) DestroyFrame(frame);
        frames.clear();

        DestroyRendererFrameBuffers(this);
        DestroySwapchain(swapchain, &swapchain.swapchain);
        DestroyCommandPool(*GRAPHICS_DATA.defaultDevice, commandPool);
        DestroySurface(this);
    }

    void CreateFrame(VkFrame* result)
    {
        CreateSemaphore(*GRAPHICS_DATA.defaultDevice, &result->isImageAvailable);
        CreateSemaphore(*GRAPHICS_DATA.defaultDevice, &result->isRenderingFinished);
        CreateFence(*GRAPHICS_DATA.defaultDevice, &result->isInFlight, true);
    }

    void DestroyFrame(VkFrame& frame)
    {
        DestroySemaphore(*GRAPHICS_DATA.defaultDevice, frame.isImageAvailable);
        DestroySemaphore(*GRAPHICS_DATA.defaultDevice, frame.isRenderingFinished);
        DestroyFence(*GRAPHICS_DATA.defaultDevice, frame.isInFlight);
    }

    //------------------------------------------------------------------------------------

    void CreateLogicalDevice(GraphicsDevice& device)
    {
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
        std::set<uint32_t> uniqueQueueFamilies =
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

        vkGetDeviceQueue(device.logicalDevice.device, device.familyIndices.graphicsFamily.value(),
            0, &device.logicalDevice.graphicsQueue);

        vkGetDeviceQueue(device.logicalDevice.device, device.familyIndices.presentFamily.value(),
            0, &device.logicalDevice.presentQueue);

        vkGetDeviceQueue(device.logicalDevice.device, device.familyIndices.transferFamily.value(),
            0, &device.logicalDevice.transferQueue);
    }

    //------------------------------------------------------------------------------------

    bool SetupPhysicalDevice(const VKRenderer* renderer, VkPhysicalDevice device, GraphicsDevice* deviceData)
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
            else if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
                deviceData->familyIndices.transferFamily = i;

            VkBool32 presentSupport = false;
            VK_HANDLE_EXCEPT(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, renderer->swapchain.surface, &presentSupport));

            if (presentSupport) deviceData->familyIndices.presentFamily = i;
            if (deviceData->familyIndices.IsComplete()) break;
        }

        if (!deviceData->familyIndices.IsComplete()) return false;

        int32_t score = 0;
        score += (deviceData->properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) * 1000;
        score += (int32_t)deviceData->properties.limits.maxImageDimension2D;
        deviceData->score = score;

        vkGetPhysicalDeviceMemoryProperties(deviceData->device, &deviceData->memProps);
        return true;
    }
}