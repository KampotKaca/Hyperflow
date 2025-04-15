#include "hvk_graphics.h"

#include <hframebuffer.h>
#include <bits/ranges_algo.h>

#include "hgenericexception.h"
#include "exceptions/hgraphicsexception.h"
#include "hyperflow.h"
#include "../config.h"
#include "hinternal.h"

namespace hf::inter::rendering
{
    GraphicsData GRAPHICS_DATA;

    //Logical devices are destroyed in loader
    static void CreateLogicalDevice(GraphicsDevice& device);
    static bool SetupPhysicalDevice(const VKRendererData* renderer, VkPhysicalDevice device, GraphicsDevice* deviceData);
    static void SetupViewportAndScissor(VKRendererData* rendererData);

    //------------------------------------------------------------------------------------

    void CreateVulkanRenderer(VKRendererData* rendererData)
    {
        SwapChainSupportDetails swapChainSupport{};
        if (!GRAPHICS_DATA.devicesAreLoaded)
        {
            uint32_t deviceCount = 0;
            VK_HANDLE_EXCEPT(vkEnumeratePhysicalDevices(GRAPHICS_DATA.instance,
                &deviceCount, nullptr));

            if (deviceCount == 0) throw GENERIC_EXCEPT("[Vulkan]", "No Graphics device found");

            std::vector<VkPhysicalDevice> availableDevices(deviceCount);
            VK_HANDLE_EXCEPT(vkEnumeratePhysicalDevices(GRAPHICS_DATA.instance,
                &deviceCount, availableDevices.data()));

            CreateSurface(rendererData);

            for (const auto& device : availableDevices)
            {
                GraphicsDevice deviceData{};
                if (SetupPhysicalDevice(rendererData, device, &deviceData) &&
                    CheckDeviceExtensionSupport(device))
                {
                    QuerySwapChainSupport(device, rendererData->surface, &swapChainSupport);

                    if (!swapChainSupport.formats.empty() &&
                        !swapChainSupport.presentModes.empty())
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

            CreateSwapchain(rendererData->surface, swapChainSupport, &rendererData->swapchain);
            SetupViewportAndScissor(rendererData);
            CreateRenderPass(&GRAPHICS_DATA.renderPass);
        }
        else
        {
            CreateSurface(rendererData);
            QuerySwapChainSupport(GRAPHICS_DATA.defaultDevice->device, rendererData->surface, &swapChainSupport);

            if (swapChainSupport.formats.empty() ||
                swapChainSupport.presentModes.empty())
                throw GENERIC_EXCEPT("[Vulkan]", "Device is not suitable!!!");

            CreateSwapchain(rendererData->surface, swapChainSupport, &rendererData->swapchain);
            SetupViewportAndScissor(rendererData);
        }

        auto& imageViews = rendererData->swapchain.imageViews;
        rendererData->swapchain.frameBuffers = std::vector<VkFrameBuffer*>(imageViews.size());
        for (uint32_t i = 0; i < imageViews.size(); ++i)
        {
            rendererData->swapchain.frameBuffers[i] = new VkFrameBuffer(&rendererData->swapchain.imageViews[i],
                1, GRAPHICS_DATA.renderPass,
                VkExtent2D
                {
                    .width = (uint32_t)rendererData->viewport.width,
                    .height = (uint32_t)rendererData->viewport.height
                });
        }

        VkCommandBuffer commandBuffer;
        CreateCommandPool(*GRAPHICS_DATA.defaultDevice, &rendererData->commandPool);
        CreateCommandBuffer(*GRAPHICS_DATA.defaultDevice, &rendererData->commandPool, &commandBuffer);
    }

    void DestroyVulkanRenderer(VKRendererData* rendererData)
    {
        DestroyCommandPool(*GRAPHICS_DATA.defaultDevice, rendererData->commandPool);
        for (auto& frameBuffer : rendererData->swapchain.frameBuffers)
            delete frameBuffer;
        rendererData->swapchain.frameBuffers.clear();
        DestroySwapchain(rendererData->swapchain);
        DestroySurface(rendererData);
    }

    void SetupViewportAndScissor(VKRendererData* rendererData)
    {
        rendererData->viewport =
        {
            .x = 0.0f,
            .y = 0.0f,
            .width = (float)rendererData->swapchain.details.extent.width,
            .height = (float)rendererData->swapchain.details.extent.height,
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };

        rendererData->scissor =
        {
            .offset = { 0, 0 },
            .extent = rendererData->swapchain.details.extent
        };
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
}