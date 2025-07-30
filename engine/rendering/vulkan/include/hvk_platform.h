#ifndef HVK_PLATFORM_H
#define HVK_PLATFORM_H

#include "hshared.h"
#include <vulkan/vulkan.h>

namespace hf
{
    struct VulkanPlatformAPI
    {
        const char* requiredExtension[16]{};
        uint32_t requiredExtensionCount = 0;
    };

    constexpr std::array DEVICE_EXTENSIONS
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
        VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
        VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
        VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
        VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,
        VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
        VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
        VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME,
    };

    static VkPhysicalDeviceBufferDeviceAddressFeaturesKHR BUFFER_DEVICE_ADDRESS_FEATURES
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR,
        .bufferDeviceAddress = VK_TRUE,
    };

    static VkPhysicalDeviceDescriptorBufferFeaturesEXT DESCRIPTOR_BUFFER_FEATURES
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_FEATURES_EXT,
        .pNext = &BUFFER_DEVICE_ADDRESS_FEATURES,
        .descriptorBuffer = VK_TRUE,
        .descriptorBufferCaptureReplay = VK_FALSE,
        .descriptorBufferImageLayoutIgnored = VK_TRUE,
        .descriptorBufferPushDescriptors = VK_FALSE
    };

    static VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT GRAPHICS_PIPELINE_LIBRARY_FEATURES
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_FEATURES_EXT,
        .pNext = &DESCRIPTOR_BUFFER_FEATURES,
        .graphicsPipelineLibrary = VK_TRUE
    };

    static VkPhysicalDeviceSynchronization2Features SYNC2_FEATURES
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
        .pNext = &GRAPHICS_PIPELINE_LIBRARY_FEATURES,
        .synchronization2 = VK_TRUE
    };

    static VkPhysicalDeviceDynamicRenderingFeaturesKHR DYNAMIC_RENDERING_FEATURES
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
        .pNext = &SYNC2_FEATURES,
        .dynamicRendering = VK_TRUE
    };

    VulkanPlatformAPI* GetAPI();
}

#endif //HVK_PLATFORM_H
