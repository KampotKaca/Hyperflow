#include "hvk_graphics.h"
#include "exceptions/hgraphicsexception.h"

namespace hf::inter::rendering
{
    void CreateSemaphore(const GraphicsDevice& device, VkSemaphore* semaphore, SemaphoreType type)
    {
        VkSemaphoreCreateInfo semaphoreInfo
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
        };

        switch (type)
        {
        case SemaphoreType::Boolean: break;
        case SemaphoreType::Timeline: break;
        default: ;
        }

        VK_HANDLE_EXCEPT(vkCreateSemaphore(device.logicalDevice.device, &semaphoreInfo, nullptr, semaphore));
    }

    void DestroySemaphore(const GraphicsDevice& device, VkSemaphore& semaphore)
    {
        vkDestroySemaphore(device.logicalDevice.device, semaphore, nullptr);
        semaphore = VK_NULL_HANDLE;
    }

    void CreateFence(const GraphicsDevice& device, VkFence* fence, bool startSignaled)
    {
        VkFenceCreateInfo fenceInfo
        {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO
        };

        if (startSignaled) fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VK_HANDLE_EXCEPT(vkCreateFence(device.logicalDevice.device, &fenceInfo, nullptr, fence));
    }

    void DestroyFence(const GraphicsDevice& device, VkFence& fence)
    {
        vkDestroyFence(device.logicalDevice.device, fence, nullptr);
        fence = VK_NULL_HANDLE;
    }

    void WaitForFences(const GraphicsDevice& device, const VkFence* fences, const uint32_t count, const bool waitAll)
    {
        vkWaitForFences(device.logicalDevice.device, count, fences, waitAll, UINT64_MAX);
        vkResetFences(device.logicalDevice.device, count, fences);
    }
}