#include "hvk_graphics.h"
#include "exceptions/hgraphicsexception.h"
#include "hgenericexception.h"

namespace hf::inter::rendering
{
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
        return GRAPHICS_DATA.availableExtensionNames.contains(extension);
    }

    bool QueueFamilyIndices::IsComplete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }
}