#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    static void DestroyExistingViews(const GraphicsSwapChain& swapchain);

    void CreateSwapchain(VkSurfaceKHR surface, uvec2 targetSize, VsyncMode vsyncMode, GraphicsSwapChain& result)
    {
        SwapChainSupportDetails scs{};
        QuerySwapChainSupport(GRAPHICS_DATA.device.device, surface, scs);

        if (scs.formats.empty() ||
            scs.presentModes.empty())
            throw GENERIC_EXCEPT("[Vulkan]", "Device is not suitable!!!");

        auto oldSwapchain = result.swapchain;
        GraphicsSwapchainDetails details{};
        VkPresentModeKHR targetPresentMode{};
        VkPresentModeKHR defaultPresentMode{};

        switch (vsyncMode)
        {
        case VsyncMode::NoSync:
            {
                targetPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
                defaultPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
            }
            break;
        case VsyncMode::Relaxed:
            {
                targetPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                defaultPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
            }
            break;
        case VsyncMode::Full:
            {
                targetPresentMode = VK_PRESENT_MODE_FIFO_KHR;
                defaultPresentMode = VK_PRESENT_MODE_FIFO_KHR;
            }
            break;
        default: throw GENERIC_EXCEPT("[Vulkan]", "Unsupported vsync mode!");
        }

        if (GetAvailableSurfaceDetails(scs, (VkFormat)VULKAN_API_COLOR_FORMAT,
            targetPresentMode, defaultPresentMode, targetSize, details))
        {
            uint32_t imageCount = scs.capabilities.minImageCount + 1;
            uint32_t maxImageCount = scs.capabilities.maxImageCount;
            if (maxImageCount > 0 && imageCount > maxImageCount) imageCount = maxImageCount;

            auto& transferData = GRAPHICS_DATA.device.transferData;
            VkSwapchainCreateInfoKHR createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            createInfo.surface = surface;
            createInfo.minImageCount = imageCount;
            createInfo.imageFormat = details.format.format;
            createInfo.imageColorSpace = details.format.colorSpace;
            createInfo.imageExtent = details.extent;
            createInfo.imageArrayLayers = 1;
            createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            createInfo.imageSharingMode = transferData.sharingMode;
            createInfo.queueFamilyIndexCount = (uint32_t)transferData.indices.size();
            createInfo.pQueueFamilyIndices = transferData.indices.data();
            createInfo.preTransform = scs.capabilities.currentTransform;
            createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            createInfo.presentMode = details.presentMode;
            createInfo.clipped = VK_TRUE;
            createInfo.oldSwapchain = oldSwapchain;

            VK_HANDLE_EXCEPT(vkCreateSwapchainKHR(GRAPHICS_DATA.device.logicalDevice.device, &createInfo,
                &GRAPHICS_DATA.platform.allocator, &result.swapchain));
            result.details = details;
        }
        else throw GENERIC_EXCEPT("[Vulkan]", "Unable to create swapchain");

        uint32_t imageCount;
        VK_HANDLE_EXCEPT(vkGetSwapchainImagesKHR(GRAPHICS_DATA.device.logicalDevice.device,
            result.swapchain, &imageCount, nullptr));

        auto images = List<VkImage>(imageCount);
        VK_HANDLE_EXCEPT(vkGetSwapchainImagesKHR(GRAPHICS_DATA.device.logicalDevice.device,
            result.swapchain, &imageCount, images.data()));

        auto imageViews = List<VkImageView>(imageCount);
        for (uint32_t i = 0; i < imageCount; i++)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = details.format.format;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.layerCount = 1;

            VK_HANDLE_EXCEPT(vkCreateImageView(GRAPHICS_DATA.device.logicalDevice.device,
                &createInfo, &GRAPHICS_DATA.platform.allocator, &imageViews[i]));
        }

        DestroySwapchain(result, &oldSwapchain);
        if (result.images.size() != imageCount) result.images = SmallList<SwapchainImage, 16>(imageCount);
        for (uint32_t i = 0; i < imageCount; i++)
        {
            auto& resImage = result.images[i];
            resImage.image = images[i];
            resImage.view = imageViews[i];
        }
    }

    void DestroySwapchain(const GraphicsSwapChain& gc, VkSwapchainKHR* swapchain)
    {
        if (*swapchain != VK_NULL_HANDLE)
        {
            DestroyExistingViews(gc);
            const auto& device = GRAPHICS_DATA.device.logicalDevice.device;
            vkDestroySwapchainKHR(device, *swapchain, &GRAPHICS_DATA.platform.allocator);
            *swapchain = VK_NULL_HANDLE;
        }
    }

    void DestroyExistingViews(const GraphicsSwapChain& swapchain)
    {
        const auto& device = GRAPHICS_DATA.device.logicalDevice.device;
        for (auto& image : swapchain.images)
            vkDestroyImageView(device, image.view, &GRAPHICS_DATA.platform.allocator);
    }

    void RecreateSwapchain(VkRenderer* rn)
    {
        rn->frameBufferChanged = false;
        WaitForDevice();

        CreateSwapchain(rn->swapchain.surface, rn->targetSize, rn->vSyncMode, rn->swapchain);
        SetupViewportAndScissor(rn);
    }

    void TryRecreateSwapchain(VkRenderer* rn)
    {
        std::lock_guard lock(rn->frameSync);
        if (rn->frameBufferChanged) RecreateSwapchain(rn);
    }

    void LockedRecreateSwapchain(VkRenderer* rn)
    {
        std::lock_guard lock(rn->frameSync);
        RecreateSwapchain(rn);
    }

    void PresentSwapchain(const VkRenderer* rn)
    {
        const auto& image = rn->swapchain.images[rn->imageIndex];
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &image.isRenderingFinished;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &rn->swapchain.swapchain;
        presentInfo.pImageIndices = &rn->imageIndex;

        const auto result = vkQueuePresentKHR(GRAPHICS_DATA.device.logicalDevice.presentQueue, &presentInfo);
        if (result != VK_SUCCESS && result != VK_ERROR_OUT_OF_DATE_KHR && result != VK_SUBOPTIMAL_KHR)
        {
            log_error("Failed to present swapchain %i", rn->currentFrame);
            VK_HANDLE_EXCEPT(result);
        }
    }

    uvec2 AcquireNextImage(VkRenderer* rn, VkRenderTexture** pTextures, uint32_t textureCount)
    {
        const auto& device = GRAPHICS_DATA.device.logicalDevice.device;
        SubmitAllOperations();
        TryRecreateSwapchain(rn);

        uint32_t tryCount = 0;
        tryAgain:
        const auto result = vkAcquireNextImageKHR(device,
                            rn->swapchain.swapchain, VULKAN_API_MAX_TIMEOUT,
                            rn->frames[rn->currentFrame].isImageAvailable, VK_NULL_HANDLE, &rn->imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            if (rn->targetSize.x == 0 || rn->targetSize.y == 0) return { 0, 0 };
            LockedRecreateSwapchain(rn);
            tryCount++;
            if (tryCount < 144) goto tryAgain;
            throw GENERIC_EXCEPT("[Vulkan]", "Unable to acquire image from swapchain!");
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            throw GENERIC_EXCEPT("[Vulkan]", "Unable to acquire image from swapchain!");

        for (uint32_t i = 0; i < textureCount; i++) ResizeRenderTexture(pTextures[i], rn->targetSize);
        return rn->targetSize;
    }

    void DelayUntilPreviousFrameFinish(VkRenderer* rn)
    {
        if (rn->imageIndex != UINT32_MAX)
        {
            const auto& device = GRAPHICS_DATA.device.logicalDevice.device;
            const auto& previousImage = rn->swapchain.images[rn->imageIndex];
            VK_HANDLE_EXCEPT(vkWaitForFences(device, 1, &previousImage.isInFlight, true, VULKAN_API_MAX_TIMEOUT));
            VK_HANDLE_EXCEPT(vkResetFences(device, 1, &previousImage.isInFlight));
        }
        else rn->imageIndex = rn->swapchain.images.size() - 1;
    }
}
