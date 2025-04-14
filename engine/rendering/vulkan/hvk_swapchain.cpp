#include "hvk_graphics.h"
#include "../config.h"
#include "exceptions/hgraphicsexception.h"
#include "hgenericexception.h"

namespace hf::inter::rendering
{
    void CreateSwapchain(VkSurfaceKHR surface, const SwapChainSupportDetails& scs, GraphicsSwapChain* result)
    {
        DestroySwapchain(*result);
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
                nullptr, &result->swapchain));
            result->details = details;
        }
        else throw GENERIC_EXCEPT("[Vulkan]", "Unable to create swapchain");

        uint32_t imageCount;
        VK_HANDLE_EXCEPT(vkGetSwapchainImagesKHR(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
            result->swapchain, &imageCount, nullptr));

        auto& images = result->images;
        images = std::vector<VkImage>(imageCount);
        VK_HANDLE_EXCEPT(vkGetSwapchainImagesKHR(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
            result->swapchain, &imageCount,
            images.data()));

        auto& imageViews = result->imageViews;
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

    void DestroySwapchain(GraphicsSwapChain& swapchain)
    {
        if (swapchain.swapchain != VK_NULL_HANDLE)
        {
            auto& device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
            for (auto& imageView : swapchain.imageViews)
                vkDestroyImageView(device, imageView, nullptr);

            swapchain.imageViews.clear();
            swapchain.images.clear();

            vkDestroySwapchainKHR(device, swapchain.swapchain, nullptr);
            swapchain.swapchain = VK_NULL_HANDLE;
        }
    }
}