#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    static constexpr uint32_t VK_MEMORY_TYPE[(uint32_t)VertBufferMemoryType::Count]
    {
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };

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
            // LOG_WARN("[Vulkan] %s", "Unable to choose target swapchain present mode, defaulted to FIFO");
        }

        return mask == (1 << 2) - 1;
    }

    bool CheckDeviceExtensionSupport(const VkPhysicalDevice& device)
    {
        uint32_t extensionCount;
        VK_HANDLE_EXCEPT(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr));

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        VK_HANDLE_EXCEPT(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data()));

        for (uint32_t i = 0; i < NUM_DEVICE_EXTENSIONS; i++)
        {
            auto& ext = DEVICE_EXTENSIONS[i];
            bool found = false;
            for (const auto& extension : availableExtensions)
            {
                if (strcmp(extension.extensionName, ext))
                {
                    found = true;
                    break;
                }
            }

            if (!found) return false;
        }

        return true;
    }

    void CreateRendererFrameBuffers(VKRenderer* rn)
    {
        auto& imageViews = rn->swapchain.imageViews;
        rn->swapchain.frameBuffers = std::vector<VkFrameBuffer*>(imageViews.size());
        for (uint32_t i = 0; i < imageViews.size(); ++i)
        {
            rn->swapchain.frameBuffers[i] = new VkFrameBuffer(&rn->swapchain.imageViews[i],
                1, GRAPHICS_DATA.renderPass,
                rn->swapchain.details.extent);
        }
    }

    void DestroyRendererFrameBuffers(VKRenderer* rn)
    {
        for (auto& frameBuffer : rn->swapchain.frameBuffers) delete frameBuffer;
        rn->swapchain.frameBuffers.clear();
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
        for (const auto& availableExtension : GRAPHICS_DATA.availableExtensions)
            if (strcmp(extension, availableExtension.extensionName) != 0) return true;
        return false;
    }

    bool QueueFamilyIndices::IsComplete() const { return graphicsFamily.has_value() && presentFamily.has_value() && transferFamily.has_value(); }

    void SetupViewportAndScissor(VKRenderer* rn)
    {
        auto& extent = rn->swapchain.details.extent;
        rn->viewport =
        {
            .x = 0.0f,
            .y = 0.0f,
            .width = (float)extent.width,
            .height = (float)extent.height,
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };

        rn->scissor =
        {
            .offset = { 0, 0 },
            .extent = extent
        };
    }

    void UploadViewportAndScissor(const VKRenderer* rn)
    {
        vkCmdSetViewport(rn->currentCommand, 0, 1, &rn->viewport);
        vkCmdSetScissor(rn->currentCommand, 0, 1, &rn->scissor);
    }

    uint32_t GetMemoryType(uint32_t filter, VkMemoryPropertyFlags props)
    {
        auto& memProps = GRAPHICS_DATA.defaultDevice->memProps;
        for (uint32_t i = 0; i < memProps.memoryTypeCount; i++)
        {
            if ((filter & (1 << i)) && (memProps.memoryTypes[i].propertyFlags & props) == props) return i;
        }

        throw GENERIC_EXCEPT("[Hyperflow]", "Unable to allocate graphics memory");
    }

    void CreateBuffer(const VkCreateBufferInfo& info, VkBuffer* bufferResult, VkDeviceMemory* memResult)
    {
        VkBufferCreateInfo bufferInfo
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = info.size,
            .usage = info.usage,
            .sharingMode = info.sharingMode,
            .queueFamilyIndexCount = info.familyCount,
            .pQueueFamilyIndices = info.pQueueFamilies
        };

        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        VK_HANDLE_EXCEPT(vkCreateBuffer(device, &bufferInfo, nullptr, bufferResult));
        VkMemoryRequirements memReqs{};
        vkGetBufferMemoryRequirements(device, *bufferResult, &memReqs);

        VkMemoryAllocateInfo allocInfo
        {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memReqs.size,
            .memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_TYPE[(uint32_t)info.memoryType])
        };

        VK_HANDLE_EXCEPT(vkAllocateMemory(device, &allocInfo, nullptr, memResult));
        VK_HANDLE_EXCEPT(vkBindBufferMemory(device, *bufferResult, *memResult, 0));
    }
}