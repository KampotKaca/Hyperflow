#include "hvk_graphics.h"

namespace hf
{
    void CreateSemaphore(const GraphicsDevice& device, VkSemaphore* semaphore)
    {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VK_HANDLE_EXCEPT(vkCreateSemaphore(device.logicalDevice.device, &semaphoreInfo, &GRAPHICS_DATA.platform.allocator, semaphore));
    }

    void DestroySemaphore(const GraphicsDevice& device, VkSemaphore& semaphore)
    {
        vkDestroySemaphore(device.logicalDevice.device, semaphore, &GRAPHICS_DATA.platform.allocator);
        semaphore = VK_NULL_HANDLE;
    }

    void CreateFence(const GraphicsDevice& device, VkFence* fence, bool startSignaled)
    {
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        if (startSignaled) fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VK_HANDLE_EXCEPT(vkCreateFence(device.logicalDevice.device, &fenceInfo, &GRAPHICS_DATA.platform.allocator, fence));
    }

    void DestroyFence(const GraphicsDevice& device, VkFence& fence)
    {
        vkDestroyFence(device.logicalDevice.device, fence, &GRAPHICS_DATA.platform.allocator);
        fence = VK_NULL_HANDLE;
    }

    void WaitForFences(const GraphicsDevice& device, const VkFence* fences, const uint32_t count, const bool waitAll)
    {
        vkWaitForFences(device.logicalDevice.device, count, fences, waitAll, VULKAN_API_MAX_TIMEOUT);
        vkResetFences(device.logicalDevice.device, count, fences);
    }
}