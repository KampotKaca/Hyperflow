#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    GraphicsData GRAPHICS_DATA;

    //Logical devices are destroyed in loader
    static void CreateLogicalDevice(GraphicsDevice& device);
    static bool SetupPhysicalDevice(VkSurfaceKHR surface, VkPhysicalDevice device, GraphicsDevice& deviceData);
    static void LoadExtensionFunctions();

    //------------------------------------------------------------------------------------

    VkRenderer::VkRenderer(const ir::rdr::RendererInstanceCreationInfo_i& info)
        : shutdownCallback(info.shutdownCallback), windowHandle(info.handle), targetSize(info.size), vSyncMode(info.vSyncMode)
    {
        if (!GRAPHICS_DATA.deviceIsLoaded) LoadDevice(windowHandle, &swapchain.surface);
        else VK_HANDLE_EXCEPT((VkResult)GRAPHICS_DATA.platform.functions.createVulkanSurfaceFunc
            (windowHandle, GRAPHICS_DATA.instance, &GRAPHICS_DATA.platform.allocator, &swapchain.surface));

        if (info.initCallback) info.initCallback();
        CreateSwapchain(swapchain.surface, targetSize, vSyncMode,  swapchain);
        SetupViewportAndScissor(this);

        CreateCommandPool(GRAPHICS_DATA.device, GRAPHICS_DATA.device.familyIndices.graphicsFamily.value(), &commandPool);
        CreateCommandBuffers(GRAPHICS_DATA.device, &commandPool, FRAMES_IN_FLIGHT);

        frames = SmallList<VkFrame, 8>(FRAMES_IN_FLIGHT);
        for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; ++i)
            CreateSemaphore(GRAPHICS_DATA.device, &frames[i].isImageAvailable);

        for (auto& image : swapchain.images)
        {
            CreateSemaphore(GRAPHICS_DATA.device, &image.isRenderingFinished);
            CreateFence(GRAPHICS_DATA.device, &image.isInFlight, true);
        }
    }

    VkRenderer::~VkRenderer()
    {
        if (shutdownCallback) shutdownCallback();

        for (auto& image : swapchain.images)
        {
            DestroySemaphore(GRAPHICS_DATA.device, image.isRenderingFinished);
            DestroyFence(GRAPHICS_DATA.device, image.isInFlight);
        }

        for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; ++i)
            DestroySemaphore(GRAPHICS_DATA.device, frames[i].isImageAvailable);
        frames.clear();

        DestroySwapchain(swapchain, &swapchain.swapchain);
        DestroyCommandPool(GRAPHICS_DATA.device, commandPool);
        DestroySurface(this);
    }

    //------------------------------------------------------------------------------------

    void CreateLogicalDevice(GraphicsDevice& device)
    {
        if (!device.features.features.robustBufferAccess)           log_warn_s("[Vulkan] Feature Not Supported %s", "Robust Buffer Access");
        if (!device.features.features.fullDrawIndexUint32)          log_warn_s("[Vulkan] Feature Not Supported %s", "Full Draw Index Uint32");
        if (!device.features.features.imageCubeArray)               log_warn_s("[Vulkan] Feature Not Supported %s", "Image Cube Array");
        if (!device.features.features.independentBlend)             log_warn_s("[Vulkan] Feature Not Supported %s", "Independent Blend");
        if (!device.features.features.sampleRateShading)            log_warn_s("[Vulkan] Feature Not Supported %s", "Sample Rate Shading");
        if (!device.features.features.dualSrcBlend)                 log_warn_s("[Vulkan] Feature Not Supported %s", "Dual Src Blend");
        if (!device.features.features.logicOp)                      log_warn_s("[Vulkan] Feature Not Supported %s", "Logic Op");
        if (!device.features.features.multiDrawIndirect)            log_warn_s("[Vulkan] Feature Not Supported %s", "Multi Draw Indirect");
        if (!device.features.features.drawIndirectFirstInstance)    log_warn_s("[Vulkan] Feature Not Supported %s", "Draw Indirect First Instance");
        if (!device.features.features.depthClamp)                   log_warn_s("[Vulkan] Feature Not Supported %s", "Depth Clamp");
        if (!device.features.features.depthBiasClamp)               log_warn_s("[Vulkan] Feature Not Supported %s", "Depth Bias Clamp");
        if (!device.features.features.fillModeNonSolid)             log_warn_s("[Vulkan] Feature Not Supported %s", "Fill Mode Non-Solid");
        if (!device.features.features.depthBounds)                  log_warn_s("[Vulkan] Feature Not Supported %s", "Depth Bounds");
        if (!device.features.features.wideLines)                    log_warn_s("[Vulkan] Feature Not Supported %s", "Wide Lines");
        if (!device.features.features.largePoints)                  log_warn_s("[Vulkan] Feature Not Supported %s", "Large Points");
        if (!device.features.features.alphaToOne)                   log_warn_s("[Vulkan] Feature Not Supported %s", "Alpha To One");
        if (!device.features.features.multiViewport)                log_warn_s("[Vulkan] Feature Not Supported %s", "Multi Viewport Features");
        if (!device.features.features.samplerAnisotropy)            log_warn_s("[Vulkan] Feature Not Supported %s", "Sampler Anisotropy");
        if (!device.features.features.textureCompressionETC2)       log_warn_s("[Vulkan] Feature Not Supported %s", "Texture Compression ETC2");
        if (!device.features.features.textureCompressionASTC_LDR)   log_warn_s("[Vulkan] Feature Not Supported %s", "Texture Compression ASTC_LDR");
        if (!device.features.features.textureCompressionBC)         log_warn_s("[Vulkan] Feature Not Supported %s", "Texture Compression BC");
        if (!device.features.features.occlusionQueryPrecise)        log_warn_s("[Vulkan] Feature Not Supported %s", "Occlusion Query Precise");
        if (!device.features.features.pipelineStatisticsQuery)      log_warn_s("[Vulkan] Feature Not Supported %s", "Pipeline Statistics Query");
        if (!device.features.features.vertexPipelineStoresAndAtomics)           log_warn_s("[Vulkan] Feature Not Supported %s", "Vertex Pipeline Stores And Atomics");
        if (!device.features.features.fragmentStoresAndAtomics)                 log_warn_s("[Vulkan] Feature Not Supported %s", "Fragment Stores And Atomics");
        if (!device.features.features.shaderTessellationAndGeometryPointSize)   log_warn_s("[Vulkan] Feature Not Supported %s", "Shader Tessellation And Geometry Point Size");
        if (!device.features.features.shaderImageGatherExtended)                log_warn_s("[Vulkan] Feature Not Supported %s", "Shader Image Gather Extended");
        if (!device.features.features.shaderStorageImageExtendedFormats)        log_warn_s("[Vulkan] Feature Not Supported %s", "Shader Storage Image Extended Formats");
        if (!device.features.features.shaderStorageImageMultisample)            log_warn_s("[Vulkan] Feature Not Supported %s", "Shader Storage Image Multisample");
        if (!device.features.features.shaderStorageImageReadWithoutFormat)      log_warn_s("[Vulkan] Feature Not Supported %s", "Shader Image Read Without Format");
        if (!device.features.features.shaderStorageImageWriteWithoutFormat)     log_warn_s("[Vulkan] Feature Not Supported %s", "Shader Image Write Without Format");
        if (!device.features.features.shaderUniformBufferArrayDynamicIndexing)  log_warn_s("[Vulkan] Feature Not Supported %s", "Shader Uniform Buffer Array Dynamic Indexing");
        if (!device.features.features.shaderSampledImageArrayDynamicIndexing)   log_warn_s("[Vulkan] Feature Not Supported %s", "Shader Sampled Image Array Dynamic Indexing");
        if (!device.features.features.shaderStorageBufferArrayDynamicIndexing)  log_warn_s("[Vulkan] Feature Not Supported %s", "Shader Storage Buffer Array Dynamic Indexing");
        if (!device.features.features.shaderStorageImageArrayDynamicIndexing)   log_warn_s("[Vulkan] Feature Not Supported %s", "Shader Storage Image Array Dynamic Indexing");
        if (!device.features.features.shaderClipDistance)       log_warn_s("[Vulkan] Feature Not Supported %s", "Shader Clip Distance");
        if (!device.features.features.shaderCullDistance)       log_warn_s("[Vulkan] Feature Not Supported %s", "Shader Cull Distance");
        if (!device.features.features.shaderFloat64)            log_warn_s("[Vulkan] Feature Not Supported %s", "Shader Float 64");
        if (!device.features.features.shaderInt64)              log_warn_s("[Vulkan] Feature Not Supported %s", "Shader Int 64");
        if (!device.features.features.shaderInt16)              log_warn_s("[Vulkan] Feature Not Supported %s", "Shader Int 16");
        if (!device.features.features.shaderResourceResidency)  log_warn_s("[Vulkan] Feature Not Supported %s", "Shader Resource Residency");
        if (!device.features.features.shaderResourceMinLod)     log_warn_s("[Vulkan] Feature Not Supported %s", "Shader Resource Min Lod");
        if (!device.features.features.sparseBinding)            log_warn_s("[Vulkan] Feature Not Supported %s", "Sparse Binding");
        if (!device.features.features.sparseResidencyBuffer)    log_warn_s("[Vulkan] Feature Not Supported %s", "Sparse Residency Buffer");
        if (!device.features.features.sparseResidencyImage2D)   log_warn_s("[Vulkan] Feature Not Supported %s", "Sparse Residency Image 2D");
        if (!device.features.features.sparseResidencyImage3D)   log_warn_s("[Vulkan] Feature Not Supported %s", "Sparse Residency Image 3D");
        if (!device.features.features.sparseResidency2Samples)  log_warn_s("[Vulkan] Feature Not Supported %s", "Sparse Residency 2 Samples");
        if (!device.features.features.sparseResidency4Samples)  log_warn_s("[Vulkan] Feature Not Supported %s", "Sparse Residency 4 Samples");
        if (!device.features.features.sparseResidency8Samples)  log_warn_s("[Vulkan] Feature Not Supported %s", "Sparse Residency 8 Samples");
        if (!device.features.features.sparseResidency16Samples) log_warn_s("[Vulkan] Feature Not Supported %s", "Sparse Residency 16 Samples");
        if (!device.features.features.sparseResidencyAliased)   log_warn_s("[Vulkan] Feature Not Supported %s", "Sparse Residency Aliased");
        if (!device.features.features.variableMultisampleRate)  log_warn_s("[Vulkan] Feature Not Supported %s", "Variable Multisample Rate");
        if (!device.features.features.inheritedQueries)         log_warn_s("[Vulkan] Feature Not Supported %s", "Inherited Queries");

        std::set uniqueQueueFamilies =
        {
            device.familyIndices.graphicsFamily.value(),
            device.familyIndices.presentFamily.value(),
            device.familyIndices.transferFamily.value()
        };
        SmallList<VkDeviceQueueCreateInfo, 64> queueCreateInfos{};
        queueCreateInfos.reserve(uniqueQueueFamilies.size());

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

        VkPhysicalDeviceFeatures2 deviceFeatures2{};
        deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        deviceFeatures2.pNext = &DYNAMIC_RENDERING_FEATURES;
        deviceFeatures2.features = device.features.features;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pNext = &deviceFeatures2;
        createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.enabledExtensionCount = DEVICE_EXTENSIONS.size();
        createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

#if defined(VK_ENABLE_VALIDATION)
        createInfo.ppEnabledLayerNames = DEBUG_VALIDATION_LAYERS;
        createInfo.enabledLayerCount = NUM_VK_VALIDATION_LAYERS;
#endif

        VK_HANDLE_EXCEPT(vkCreateDevice(device.device, &createInfo, &GRAPHICS_DATA.platform.allocator, &device.logicalDevice.device));

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

    bool SetupPhysicalDevice(VkSurfaceKHR surface, VkPhysicalDevice device, GraphicsDevice& deviceData)
    {
        deviceData.device = device;
        deviceData.properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        deviceData.properties.pNext = &deviceData.descBufferProperties;

        deviceData.features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

        deviceData.descBufferProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_PROPERTIES_EXT;

        vkGetPhysicalDeviceProperties2(device, &deviceData.properties);
        vkGetPhysicalDeviceFeatures2(device, &deviceData.features);

        if (!deviceData.features.features.geometryShader ||
            !deviceData.features.features.tessellationShader) return false;

        uint32_t familyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);

        SmallList<VkQueueFamilyProperties, 64> queueFamilies(familyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilies.size(); i++)
        {
            auto& flags = queueFamilies[i].queueFlags;
            if (flags & VK_QUEUE_GRAPHICS_BIT)      deviceData.familyIndices.graphicsFamily = i;
            else if (flags & VK_QUEUE_TRANSFER_BIT) deviceData.familyIndices.transferFamily = i;
            else if (flags & VK_QUEUE_COMPUTE_BIT)  deviceData.familyIndices.computeFamily = i;

            VkBool32 presentSupport = false;
            VK_HANDLE_EXCEPT(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport));

            if (presentSupport) deviceData.familyIndices.presentFamily = i;
            if (deviceData.familyIndices.IsComplete()) break;
        }

        auto& indices = deviceData.familyIndices;
        if (!indices.computeFamily.has_value() && indices.graphicsFamily.has_value())
        {
            auto& graphicsFlags = queueFamilies[indices.graphicsFamily.value()].queueFlags;
            if (graphicsFlags & VK_QUEUE_COMPUTE_BIT)
                deviceData.familyIndices.computeFamily = indices.graphicsFamily.value();
            else return false;
        }

        if (!indices.IsComplete()) return false;

        if (indices.graphicsFamily != indices.presentFamily)
        {
            std::set uniqueIndices = { indices.transferFamily.value(), indices.graphicsFamily.value(), indices.presentFamily.value(), indices.computeFamily.value() };
            deviceData.transferData.indices = SmallList<uint32_t, 4>(uniqueIndices.begin(), uniqueIndices.end());

            if (deviceData.transferData.indices.size() > 1) deviceData.transferData.sharingMode = VK_SHARING_MODE_CONCURRENT;
            else deviceData.transferData.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }
        else deviceData.transferData =
        {
            .indices = {},
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE
        };

        const auto& limits = deviceData.properties.properties.limits;
        const VkSampleCountFlags maxMsaa = limits.framebufferColorSampleCounts & limits.framebufferDepthSampleCounts;

        int32_t score = 0;
        score += (deviceData.properties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) * 1000;
        score += (int32_t)deviceData.properties.properties.limits.maxImageDimension2D;
        score += (int32_t)maxMsaa;
        deviceData.score = score;

        vkGetPhysicalDeviceMemoryProperties(deviceData.device, &deviceData.memProps);

        for (uint32_t i = 0; i < (uint32_t)TextureFormat::Count; ++i)
            vkGetPhysicalDeviceFormatProperties(deviceData.device, (VkFormat)i, &deviceData.formatProps[i]);
        return true;
    }

    void LoadDevice(void* windowHandle, VkSurfaceKHR* resultSurface)
    {
        uint32_t deviceCount = 0;
        VK_HANDLE_EXCEPT(vkEnumeratePhysicalDevices(GRAPHICS_DATA.instance, &deviceCount, nullptr));

        if (deviceCount == 0) throw GENERIC_EXCEPT("[Vulkan]", "No Graphics device found");

        SmallList<VkPhysicalDevice, 8> availableDevices(deviceCount);
        VK_HANDLE_EXCEPT(vkEnumeratePhysicalDevices(GRAPHICS_DATA.instance,
        &deviceCount, availableDevices.data()));

        VK_HANDLE_EXCEPT((VkResult)GRAPHICS_DATA.platform.functions.createVulkanSurfaceFunc
        (windowHandle, GRAPHICS_DATA.instance, &GRAPHICS_DATA.platform.allocator, resultSurface));

        SmallList<GraphicsDevice, 8> devices{};

        for (const auto& device : availableDevices)
        {
            GraphicsDevice deviceData{};
            if (SetupPhysicalDevice(*resultSurface, device, deviceData) &&
                CheckDeviceExtensionSupport(device))
            {
                SwapChainSupportDetails scs{};
                QuerySwapChainSupport(device, *resultSurface, scs);
                if (!scs.formats.empty() && !scs.presentModes.empty()) devices.push_back(deviceData);
            }
        }

        if (devices.empty()) throw GENERIC_EXCEPT("[Vulkan]", "No suitable graphics device found");

        uint32_t chosenDevice = 0;
        {
            uint32_t score = devices[0].score;
            for (uint32_t i = 1; i < devices.size(); ++i)
            {
                if (devices[i].score > score)
                {
                    chosenDevice = i;
                    score = devices[i].score;
                }
            }
        }

        GRAPHICS_DATA.device = devices[chosenDevice];

        log_trace_s("[Vulkan] Chosen Device: %s, Is Discrete: %i", GRAPHICS_DATA.device.properties.properties.deviceName,
                    GRAPHICS_DATA.device.properties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
        CreateLogicalDevice(GRAPHICS_DATA.device);
        GRAPHICS_DATA.deviceIsLoaded = true;

        log_trace_s("[Vulkan] Graphics device found [%s]", GRAPHICS_DATA.device.properties.properties.deviceName);

        VmaAllocatorCreateInfo allocatorInfo{};
        allocatorInfo.physicalDevice = GRAPHICS_DATA.device.device;
        allocatorInfo.device = GRAPHICS_DATA.device.logicalDevice.device;
        allocatorInfo.instance = GRAPHICS_DATA.instance;
        allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        allocatorInfo.pAllocationCallbacks = &GRAPHICS_DATA.platform.allocator;

        VK_HANDLE_EXCEPT(vmaCreateAllocator(&allocatorInfo, &GRAPHICS_DATA.allocator));

        CreateCommandPool(GRAPHICS_DATA.device, GRAPHICS_DATA.device.familyIndices.transferFamily.value(), &GRAPHICS_DATA.transferPool);
        CreateCommandBuffers(GRAPHICS_DATA.device, &GRAPHICS_DATA.transferPool, 1);

        CreateCommandPool(GRAPHICS_DATA.device, GRAPHICS_DATA.device.familyIndices.graphicsFamily.value(), &GRAPHICS_DATA.graphicsPool);
        CreateCommandBuffers(GRAPHICS_DATA.device, &GRAPHICS_DATA.graphicsPool, 1);

        LoadExtensionFunctions();

        {
            std::array types
            {
                VkDescriptorTypeCount{ .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = VK_MAX_STORAGE_BUFFER_DESCRIPTORS },
                VkDescriptorTypeCount{ .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = VK_MAX_UNIFORM_BUFFER_DESCRIPTORS },
            };
            GRAPHICS_DATA.bufferDescriptorBuffer = MakeURef<VkDescriptorBuffer>(types.data(), types.size(), VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT);
        }

        {
            std::array types
            {
                VkDescriptorTypeCount{ .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = VK_MAX_COMBINED_IMAGE_SAMPLER_DESCRIPTORS },
            };
            GRAPHICS_DATA.imageDescriptorBuffer = MakeURef<VkDescriptorBuffer>(types.data(), types.size(), VK_BUFFER_USAGE_SAMPLER_DESCRIPTOR_BUFFER_BIT_EXT);
        }

        log_trace_s("[Vulkan] Max vertex attributes supported: %i", GRAPHICS_DATA.device.properties.properties.limits.maxVertexInputAttributes);
        log_trace_s("[Vulkan] Max image array layers: %i",          GRAPHICS_DATA.device.properties.properties.limits.maxImageArrayLayers);
        log_trace_s("[Vulkan] Max texel buffer elements: %i",       GRAPHICS_DATA.device.properties.properties.limits.maxTexelBufferElements);
        log_trace_s("[Vulkan] Max uniform buffer range: %i",        GRAPHICS_DATA.device.properties.properties.limits.maxUniformBufferRange);
        log_trace_s("[Vulkan] Max storage buffer range: %i",        GRAPHICS_DATA.device.properties.properties.limits.maxStorageBufferRange);
        log_trace_s("[Vulkan] Max push constant size: %i",          GRAPHICS_DATA.device.properties.properties.limits.maxPushConstantsSize);
        log_trace_s("[Vulkan] Max color attachments: %i",           GRAPHICS_DATA.device.properties.properties.limits.maxColorAttachments);
    }

    static void DestroyLogicalDevice(LogicalDevice& device)
    {
        if (device.device != VK_NULL_HANDLE)
        {
            vkDestroyDevice(device.device, &GRAPHICS_DATA.platform.allocator);
            device.device = VK_NULL_HANDLE;
        }
    }

    void UnloadDevice()
    {
        GRAPHICS_DATA.bufferDescriptorBuffer = nullptr;
        GRAPHICS_DATA.imageDescriptorBuffer = nullptr;

        vmaDestroyAllocator(GRAPHICS_DATA.allocator);
        DestroyCommandPool(GRAPHICS_DATA.device, GRAPHICS_DATA.transferPool);
        DestroyCommandPool(GRAPHICS_DATA.device, GRAPHICS_DATA.graphicsPool);
        DestroyLogicalDevice(GRAPHICS_DATA.device.logicalDevice);
    }

    void LoadExtensionFunctions()
    {
        const auto device = GRAPHICS_DATA.device.logicalDevice.device;
        GRAPHICS_DATA.extensionFunctions.vkCmdBeginRenderingKHR             = (PFN_vkCmdBeginRenderingKHR)vkGetDeviceProcAddr(device, "vkCmdBeginRenderingKHR");
        GRAPHICS_DATA.extensionFunctions.vkCmdEndRenderingKHR               = (PFN_vkCmdEndRenderingKHR)vkGetDeviceProcAddr(device, "vkCmdEndRenderingKHR");
        GRAPHICS_DATA.extensionFunctions.vkCmdPipelineBarrier2KHR           = (PFN_vkCmdPipelineBarrier2KHR)vkGetDeviceProcAddr(device, "vkCmdPipelineBarrier2KHR");
        GRAPHICS_DATA.extensionFunctions.vkCmdBindDescriptorBuffersEXT      = (PFN_vkCmdBindDescriptorBuffersEXT)vkGetDeviceProcAddr(device, "vkCmdBindDescriptorBuffersEXT");
        GRAPHICS_DATA.extensionFunctions.vkCmdSetDescriptorBufferOffsetsEXT = (PFN_vkCmdSetDescriptorBufferOffsetsEXT)vkGetDeviceProcAddr(device, "vkCmdSetDescriptorBufferOffsetsEXT");
        GRAPHICS_DATA.extensionFunctions.vkGetDescriptorEXT                 = (PFN_vkGetDescriptorEXT)vkGetDeviceProcAddr(device, "vkGetDescriptorEXT");
        GRAPHICS_DATA.extensionFunctions.vkGetDescriptorSetLayoutSizeEXT    = (PFN_vkGetDescriptorSetLayoutSizeEXT)vkGetDeviceProcAddr(device, "vkGetDescriptorSetLayoutSizeEXT");
        GRAPHICS_DATA.extensionFunctions.vkGetBufferDeviceAddressKHR        = (PFN_vkGetBufferDeviceAddressEXT)vkGetDeviceProcAddr(device, "vkGetBufferDeviceAddressKHR");

        if (!GRAPHICS_DATA.extensionFunctions.vkCmdBeginRenderingKHR             ||
            !GRAPHICS_DATA.extensionFunctions.vkCmdEndRenderingKHR               ||
            !GRAPHICS_DATA.extensionFunctions.vkCmdPipelineBarrier2KHR           ||
            !GRAPHICS_DATA.extensionFunctions.vkCmdBindDescriptorBuffersEXT      ||
            !GRAPHICS_DATA.extensionFunctions.vkCmdSetDescriptorBufferOffsetsEXT ||
            !GRAPHICS_DATA.extensionFunctions.vkGetDescriptorEXT                 ||
            !GRAPHICS_DATA.extensionFunctions.vkGetDescriptorSetLayoutSizeEXT    ||
            !GRAPHICS_DATA.extensionFunctions.vkGetBufferDeviceAddressKHR)
            throw GENERIC_EXCEPT("[Vulkan]", "Failed to load extension functions");
    }
}