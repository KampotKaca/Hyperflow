#include "hvk_graphics.h"
#include "hvk_renderer.h"
#include "../config.h"
#include "exceptions/hgraphicsexception.h"
#include "hgenericexception.h"
#include "hinternal.h"

namespace hf::inter::rendering
{
    static void DestroyExistingViews(GraphicsSwapChain& swapchain);

    void CreateSwapchain(VkSurfaceKHR surface, uvec2 targetSize, GraphicsSwapChain* result)
    {
        SwapChainSupportDetails scs{};
        QuerySwapChainSupport(GRAPHICS_DATA.defaultDevice->device, surface, &scs);

        if (scs.formats.empty() ||
            scs.presentModes.empty())
            throw GENERIC_EXCEPT("[Vulkan]", "Device is not suitable!!!");

        auto oldSwapchain = result->swapchain;
        GraphicsSwapchainDetails details{};
        if (GetAvailableSurfaceDetails(scs,
            VULKAN_API_COLOR_FORMAT, VULKAN_API_PRESENT_MODE, targetSize, &details))
        {
            uint32_t imageCount = scs.capabilities.minImageCount + 1;
            uint32_t maxImageCount = scs.capabilities.maxImageCount;
            if (maxImageCount > 0 && imageCount > maxImageCount) imageCount = maxImageCount;

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
                .preTransform = scs.capabilities.currentTransform,
                .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                .presentMode = details.presentMode,
                .clipped = VK_TRUE,
                .oldSwapchain = oldSwapchain
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
                nullptr, &result->swapchain));
            result->details = details;
        }
        else throw GENERIC_EXCEPT("[Vulkan]", "Unable to create swapchain");

        uint32_t imageCount;
        VK_HANDLE_EXCEPT(vkGetSwapchainImagesKHR(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
            result->swapchain, &imageCount, nullptr));

        auto images = std::vector<VkImage>(imageCount);
        VK_HANDLE_EXCEPT(vkGetSwapchainImagesKHR(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
            result->swapchain, &imageCount, images.data()));

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

            VK_HANDLE_EXCEPT(vkCreateImageView(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
                &createInfo, nullptr, &imageViews[i]));
        }

        DestroySwapchain(*result, &oldSwapchain);
        result->images = images;
        result->imageViews = imageViews;
    }

    void DestroyExistingViews(GraphicsSwapChain& swapchain)
    {
        auto& device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        for (auto& imageView : swapchain.imageViews)
            vkDestroyImageView(device, imageView, nullptr);

        swapchain.imageViews.clear();
        swapchain.images.clear();
    }

    void DestroySwapchain(GraphicsSwapChain& gc, VkSwapchainKHR* swapchain)
    {
        if (*swapchain != VK_NULL_HANDLE)
        {
            DestroyExistingViews(gc);
            auto& device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
            vkDestroySwapchainKHR(device, *swapchain, nullptr);
            *swapchain = VK_NULL_HANDLE;
        }
    }

    void RecreateSwapchain(VKRenderer* rn)
    {
        rn->frameBufferResized = false;
        WaitForRendering();

        DestroyRendererFrameBuffers(rn);
        CreateSwapchain(rn->swapchain.surface, rn->targetSize, &rn->swapchain);
        SetupViewportAndScissor(rn);
        CreateRendererFrameBuffers(rn);
    }

    void PresentSwapchain(VKRenderer* rn)
    {
        auto frame = rn->frames[rn->currentFrame];
        VkPresentInfoKHR presentInfo
        {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &frame.isRenderingFinished,
            .swapchainCount = 1,
            .pSwapchains = &rn->swapchain.swapchain,
            .pImageIndices = &rn->imageIndex,
            .pResults = nullptr,
        };

        auto result = vkQueuePresentKHR(GRAPHICS_DATA.defaultDevice->logicalDevice.presentQueue, &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || rn->frameBufferResized)
            RecreateSwapchain(rn);
        else if (result != VK_SUCCESS) throw GENERIC_EXCEPT("[Vulkan]", "Failed to present swapchain");
    }

    bool AcquireNextImage(VKRenderer* rn)
    {
        auto& frame = rn->frames[rn->currentFrame];
        auto& device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;

        tryAgain:
        vkWaitForFences(device, 1, &frame.isInFlight, true, VULKAN_API_MAX_TIMEOUT);

        uint32_t tryCount = 0;
        auto result = vkAcquireNextImageKHR(device,
                            rn->swapchain.swapchain, VULKAN_API_MAX_TIMEOUT,
                            frame.isImageAvailable, VK_NULL_HANDLE, &rn->imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            if (rn->targetSize.x == 0 || rn->targetSize.y == 0) return false;
            RecreateSwapchain(rn);
            tryCount++;
            if (tryCount < 3) goto tryAgain;
            LOG_WARN("Recreating swapchain failed 3 times");
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            throw GENERIC_EXCEPT("[Vulkan]", "Unable to acquire image from swapchain!");

        vkResetFences(device, 1, &frame.isInFlight);
        return true;
    }
}
