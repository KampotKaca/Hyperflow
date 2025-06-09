#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    bool GetAvailableSurfaceDetails(const SwapChainSupportDetails& swapChainSupportDetails,
    VkFormat targetFormat, VkPresentModeKHR targetPresentMode, VkPresentModeKHR defaultPresentMode,
    uvec2 targetExtents, GraphicsSwapchainDetails* result)
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
            result->presentMode = defaultPresentMode;
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

        for (uint32_t i = 0; i < GRAPHICS_DATA.platform.api->deviceExtensionCount; i++)
        {
            auto& ext = GRAPHICS_DATA.platform.api->deviceExtension[i];
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

    void CreateSwapchainFrameBuffers(VkRenderer* rn)
    {
        auto& swapchain = rn->swapchain;
        uint32_t size = 0;
        uint32_t msaaCount = 0;
        for (uint32_t i = 0; i < rn->passTextureCollections.size(); ++i)
        {
            size += rn->passTextureCollections[i].depthTextures.size();
            msaaCount += rn->passTextureCollections[i].msaaTextures.size();
        }
        size += msaaCount;

        std::vector<VkImageView> views(1 + size);

        for (uint32_t i = 0; i < swapchain.images.size(); ++i)
        {
            uint32_t viewIndex = 0;
            if (msaaCount > 0)
            {
                for (uint32_t j = 0; j < rn->passTextureCollections.size(); ++j)
                {
                    auto& collection = rn->passTextureCollections[j];
                    for (uint32_t k = 0; k < collection.msaaTextures.size(); ++k)
                    {
                        views[viewIndex] = collection.msaaTextures[k]->view;
                        viewIndex++;
                    }
                }

                for (uint32_t j = 0; j < rn->passTextureCollections.size(); ++j)
                {
                    auto& collection = rn->passTextureCollections[j];
                    for (uint32_t k = 0; k < collection.depthTextures.size(); ++k)
                    {
                        views[viewIndex] = collection.depthTextures[k]->view;
                        viewIndex++;
                    }
                }
                views[viewIndex] = swapchain.images[i].view;
            }
            else
            {
                views[viewIndex] = swapchain.images[i].view;
                viewIndex++;
                for (uint32_t j = 0; j < rn->passTextureCollections.size(); ++j)
                {
                    auto& collection = rn->passTextureCollections[j];
                    for (uint32_t k = 0; k < collection.depthTextures.size(); ++k)
                    {
                        views[viewIndex] = collection.depthTextures[k]->view;
                        viewIndex++;
                    }
                }
            }

            swapchain.images[i].frameBuffer = new VkFrameBuffer(views.data(), views.size(),
                rn->mainPass, swapchain.details.extent);
        }
    }

    void DestroySwapchainFrameBuffers(VkRenderer* rn)
    {
        for (auto& image : rn->swapchain.images) delete image.frameBuffer;
    }

    void QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, SwapChainSupportDetails& supportDetails)
    {
        VK_HANDLE_EXCEPT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &supportDetails.capabilities));

        uint32_t formatCount;
        VK_HANDLE_EXCEPT(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr));

        if (formatCount > 0)
        {
            supportDetails.formats = std::vector<VkSurfaceFormatKHR>(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                supportDetails.formats.data());
        }

        uint32_t presentModeCount;
        VK_HANDLE_EXCEPT(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr));

        if (presentModeCount > 0)
        {
            supportDetails.presentModes = std::vector<VkPresentModeKHR>(presentModeCount);
            VK_HANDLE_EXCEPT(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount,
                supportDetails.presentModes.data()));
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

    bool QueueFamilyIndices::IsComplete() const { return graphicsFamily.has_value() && presentFamily.has_value() && transferFamily.has_value() && computeFamily.has_value(); }

    void SetupViewportAndScissor(VkRenderer* rn)
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

    void UploadViewportAndScissor(const VkRenderer* rn)
    {
        vkCmdSetViewport(rn->currentCommand, 0, 1, &rn->viewport);
        vkCmdSetScissor(rn->currentCommand, 0, 1, &rn->scissor);
    }
}