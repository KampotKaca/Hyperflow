#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    static void DestroyExistingViews(GraphicsSwapChain& swapchain);

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

        if (GetAvailableSurfaceDetails(scs, VULKAN_API_COLOR_FORMAT,
            targetPresentMode, defaultPresentMode, targetSize, &details))
        {
            uint32_t imageCount = scs.capabilities.minImageCount + 1;
            uint32_t maxImageCount = scs.capabilities.maxImageCount;
            if (maxImageCount > 0 && imageCount > maxImageCount) imageCount = maxImageCount;

            auto& transferData = GRAPHICS_DATA.device.transferData;
            VkSwapchainCreateInfoKHR createInfo
            {
                .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .surface = surface,
                .minImageCount = imageCount,
                .imageFormat = details.format.format,
                .imageColorSpace = details.format.colorSpace,
                .imageExtent = details.extent,
                .imageArrayLayers = 1,
                .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                .imageSharingMode = transferData.sharingMode,
                .queueFamilyIndexCount = (uint32_t)transferData.indices.size(),
                .pQueueFamilyIndices = transferData.indices.data(),
                .preTransform = scs.capabilities.currentTransform,
                .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                .presentMode = details.presentMode,
                .clipped = VK_TRUE,
                .oldSwapchain = oldSwapchain
            };

            VK_HANDLE_EXCEPT(vkCreateSwapchainKHR(GRAPHICS_DATA.device.logicalDevice.device, &createInfo,
                nullptr, &result.swapchain));
            result.details = details;
        }
        else throw GENERIC_EXCEPT("[Vulkan]", "Unable to create swapchain");

        uint32_t imageCount;
        VK_HANDLE_EXCEPT(vkGetSwapchainImagesKHR(GRAPHICS_DATA.device.logicalDevice.device,
            result.swapchain, &imageCount, nullptr));

        auto images = std::vector<VkImage>(imageCount);
        VK_HANDLE_EXCEPT(vkGetSwapchainImagesKHR(GRAPHICS_DATA.device.logicalDevice.device,
            result.swapchain, &imageCount, images.data()));

        auto imageViews = std::vector<VkImageView>(imageCount);
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

            VK_HANDLE_EXCEPT(vkCreateImageView(GRAPHICS_DATA.device.logicalDevice.device,
                &createInfo, nullptr, &imageViews[i]));
        }

        DestroySwapchain(result, &oldSwapchain);
        if (result.images.size() != imageCount) result.images = std::vector<SwapchainImage>(imageCount);
        for (uint32_t i = 0; i < imageCount; i++)
        {
            auto& resImage = result.images[i];
            resImage.image = images[i];
            resImage.view = imageViews[i];
        }
    }

    void DestroySwapchain(GraphicsSwapChain& gc, VkSwapchainKHR* swapchain)
    {
        if (*swapchain != VK_NULL_HANDLE)
        {
            DestroyExistingViews(gc);
            auto& device = GRAPHICS_DATA.device.logicalDevice.device;
            vkDestroySwapchainKHR(device, *swapchain, nullptr);
            *swapchain = VK_NULL_HANDLE;
        }
    }

    void DestroyExistingViews(GraphicsSwapChain& swapchain)
    {
        auto& device = GRAPHICS_DATA.device.logicalDevice.device;
        for (auto& image : swapchain.images)
            vkDestroyImageView(device, image.view, nullptr);
    }

    void RecreateSwapchain(VkRenderer* rn)
    {
        rn->frameBufferChanged = false;
        WaitForDevice();

        DestroySwapchainFrameBuffers(rn);
        CreateSwapchain(rn->swapchain.surface, rn->targetSize, rn->vSyncMode, rn->swapchain);
        SetupViewportAndScissor(rn);
        RebindRendererToAllPasses(rn);
        CreateSwapchainFrameBuffers(rn);
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

    void PresentSwapchain(VkRenderer* rn)
    {
        auto& image = rn->swapchain.images[rn->imageIndex];
        VkPresentInfoKHR presentInfo
        {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &image.isRenderingFinished,
            .swapchainCount = 1,
            .pSwapchains = &rn->swapchain.swapchain,
            .pImageIndices = &rn->imageIndex,
            .pResults = nullptr,
        };

        auto result = vkQueuePresentKHR(GRAPHICS_DATA.device.logicalDevice.presentQueue, &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
            LockedRecreateSwapchain(rn);
        else if (result != VK_SUCCESS)
        {
            LOG_ERROR("Failed to present swapchain %i", rn->currentFrame);
            VK_HANDLE_EXCEPT(result);
        }
    }

    bool AcquireNextImage(VkRenderer* rn)
    {
        auto& device = GRAPHICS_DATA.device.logicalDevice.device;
        SubmitAllOperations();
        TryRecreateSwapchain(rn);

        uint32_t tryCount = 0;
        tryAgain:
        auto result = vkAcquireNextImageKHR(device,
                            rn->swapchain.swapchain, VULKAN_API_MAX_TIMEOUT,
                            rn->frames[rn->currentFrame].isImageAvailable, VK_NULL_HANDLE, &rn->imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            if (rn->targetSize.x == 0 || rn->targetSize.y == 0) return false;
            LockedRecreateSwapchain(rn);
            tryCount++;
            if (tryCount < 144) goto tryAgain;
            LOG_WARN("Recreating swapchain failed 144 times");
            return false;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            throw GENERIC_EXCEPT("[Vulkan]", "Unable to acquire image from swapchain!");

        return true;
    }

    void DelayUntilPreviousFrameFinish(VkRenderer* rn)
    {
        if (rn->imageIndex != UINT32_MAX)
        {
            auto& device = GRAPHICS_DATA.device.logicalDevice.device;
            auto& previousImage = rn->swapchain.images[rn->imageIndex];
            vkWaitForFences(device, 1, &previousImage.isInFlight, true, VULKAN_API_MAX_TIMEOUT);
            vkResetFences(device, 1, &previousImage.isInFlight);
        }
        else rn->imageIndex = rn->swapchain.images.size() - 1;
    }
}
