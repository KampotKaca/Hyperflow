#include "hvk_graphics.h"

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

    static void CreateSwapchain(VKRendererData* rendererData, const SwapChainSupportDetails& scs);
    static void DestroySwapchain(VKRendererData* rendererData);

    static bool SetupPhysicalDevice(const VKRendererData* renderer, VkPhysicalDevice device, GraphicsDevice* deviceData);
    static bool CheckDeviceExtensionSupport(const VkPhysicalDevice& device);

    static void QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, SwapChainSupportDetails* supportDetails);
    static bool GetAvailableSurfaceDetails(const SwapChainSupportDetails& swapChainSupportDetails,
                                    VkFormat targetFormat, VkPresentModeKHR targetPresentMode, uvec2 targetExtents,
                                    GraphicsSwapchainDetails* result);

    bool IsLayerSupported(const char* layer)
    {
        for (const auto& availableLayer : GRAPHICS_DATA.availableLayers)
        {
            if (strcmp(layer, availableLayer.layerName) == 0) return true;
        }
        return false;
    }

    bool IsExtensionSupported(const char* extension)
    {
        return GRAPHICS_DATA.availableExtensionNames.contains(extension);
    }

    //------------------------------------------------------------------------------------

    void CreateSurface(VKRendererData* rendererData)
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

            CreatePlatformSurface(rendererData);

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
        }
        else
        {
            CreatePlatformSurface(rendererData);
            QuerySwapChainSupport(GRAPHICS_DATA.defaultDevice->device, rendererData->surface, &swapChainSupport);
        }
        CreateSwapchain(rendererData, swapChainSupport);
    }

    void DestroySurface(VKRendererData* rendererData)
    {
        DestroySwapchain(rendererData);
        DestroyPlatformSurface(rendererData);
    }

    uint32_t GetDeviceCount(const VKRendererData* rendererData) { return GRAPHICS_DATA.suitableDevices.size(); }

    void CreateSwapchain(VKRendererData* rendererData, const SwapChainSupportDetails& scs)
    {
        DestroySwapchain(rendererData);
        GRAPHICS_DATA.defaultDevice = &GRAPHICS_DATA.suitableDevices[0];
        LOG_LOG("Graphics device found [%s]", GRAPHICS_DATA.defaultDevice->properties.deviceName);

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

            auto& indices = GRAPHICS_DATA.defaultDevice->familyIndices;
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

            VK_HANDLE_EXCEPT(vkCreateSwapchainKHR(GRAPHICS_DATA.defaultDevice->logicalDevice.device, &createInfo,
                nullptr, &rendererData->swapchain.swapchain));
            rendererData->swapchain.details = details;
        }
        else throw GENERIC_EXCEPT("[Vulkan]", "Unable to create swapchain");

        uint32_t imageCount;
        VK_HANDLE_EXCEPT(vkGetSwapchainImagesKHR(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
            rendererData->swapchain.swapchain, &imageCount, nullptr));

        auto& images = rendererData->swapchain.images;
        images = std::vector<VkImage>(imageCount);
        VK_HANDLE_EXCEPT(vkGetSwapchainImagesKHR(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
            rendererData->swapchain.swapchain, &imageCount,
            images.data()));

        auto& imageViews = rendererData->swapchain.imageViews;
        imageViews = std::vector<VkImageView>(imageCount);
        for (uint32_t i = 0; i < imageCount; i++)
        {
            VkImageViewCreateInfo createInfo
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = images[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = details.format.format,
                .components =
                {
                    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .a = VK_COMPONENT_SWIZZLE_IDENTITY,
                },
                .subresourceRange =
                {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                }
            };

            VK_HANDLE_EXCEPT(vkCreateImageView(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
                &createInfo, nullptr, &imageViews[i]));
        }

    }

    void DestroySwapchain(VKRendererData* rendererData)
    {
        if (rendererData->swapchain.swapchain != VK_NULL_HANDLE)
        {
            auto& device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
            for (uint32_t i = 0; i < rendererData->swapchain.imageViews.size(); i++)
                vkDestroyImageView(device, rendererData->swapchain.imageViews[i], nullptr);

            rendererData->swapchain.imageViews.clear();
            rendererData->swapchain.images.clear();

            vkDestroySwapchainKHR(device, rendererData->swapchain.swapchain, nullptr);
            rendererData->swapchain.swapchain = VK_NULL_HANDLE;
        }
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

    void CreatePipeline(const VkPipelineCreationInfo& info, VkPipelineLayout* result)
    {
        VkPipelineVertexInputStateCreateInfo vertexInputInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = 0,
            .pVertexBindingDescriptions = nullptr,
            .vertexAttributeDescriptionCount = 0,
            .pVertexAttributeDescriptions = nullptr
        };

        VkPipelineInputAssemblyStateCreateInfo inputAssembly
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE
        };

        std::vector<VkDynamicState> dynamicStates =
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicState
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = (uint32_t)dynamicStates.size(),
            .pDynamicStates = dynamicStates.data()
        };

        VkPipelineViewportStateCreateInfo viewportState
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .pViewports = &info.viewport,
            .scissorCount = 1,
            .pScissors = &info.scissor,
        };

        VkPipelineRasterizationStateCreateInfo rasterizer
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .lineWidth = 1.0f,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
        };

        VkPipelineMultisampleStateCreateInfo multisampling
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .sampleShadingEnable = VK_FALSE,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .minSampleShading = 1.0f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE,
        };

        VkPipelineColorBlendAttachmentState colorBlendAttachment
        {
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        };

        if (info.blendingMode == PipelineBlendType::Alpha)
        {
            colorBlendAttachment.blendEnable = VK_TRUE;
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        }
        else if (info.blendingMode == PipelineBlendType::None)
        {
            colorBlendAttachment.blendEnable = VK_FALSE;
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        }

        VkPipelineColorBlendStateCreateInfo colorBlending
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = info.blendingMode == PipelineBlendType::Logical,
            .logicOp = VK_LOGIC_OP_XOR,
            .attachmentCount = 1,
            .pAttachments = &colorBlendAttachment,
            .blendConstants[0] = 0.0f,
            .blendConstants[1] = 0.0f,
            .blendConstants[2] = 0.0f,
            .blendConstants[3] = 0.0f,
        };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 0,
            .pSetLayouts = nullptr,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr,
        };

        // VK_HANDLE_EXCEPT(vkCreatePipelineLayout(info.device, &pipelineLayoutInfo, nullptr, ));
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
        if (!(mask & (1 << 1)))
        {
            result->presentMode = VK_PRESENT_MODE_FIFO_KHR;
            mask |= 1 << 1;
            LOG_WARN("[Vulkan] %s", "Unable to choose target swapchain present mode, defaulted to FIFO");
        }

        return mask == (1 << 2) - 1;
    }

    bool QueueFamilyIndices::IsComplete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }
}

namespace hf::inter::rendering
{

}