#include "hvk_graphics.h"
#include "exceptions/hgraphicsexception.h"

namespace hf::inter::rendering
{
    void CreateCommandPool(const GraphicsDevice& device, CommandPool* result)
    {
        VkCommandPoolCreateInfo poolCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = device.familyIndices.graphicsFamily.value()
        };

        VK_HANDLE_EXCEPT(vkCreateCommandPool(device.logicalDevice.device, &poolCreateInfo, nullptr, &result->pool));
    }

    void DestroyCommandPool(const GraphicsDevice& device, CommandPool& pool)
    {
        vkDestroyCommandPool(device.logicalDevice.device, pool.pool, nullptr);
        pool.pool = VK_NULL_HANDLE;
    }

    void CreateCommandBuffer(const GraphicsDevice& device, CommandPool* pool, VkCommandBuffer* result)
    {
        VkCommandBufferAllocateInfo allocInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = pool->pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };

        VK_HANDLE_EXCEPT(vkAllocateCommandBuffers(device.logicalDevice.device, &allocInfo, result));
        pool->buffers.push_back(*result);
    }

    void BeginCommandBuffer(VKRendererData* rn, VkCommandBuffer buffer)
    {
        vkResetCommandBuffer(buffer, 0);
        VkCommandBufferBeginInfo beginInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        };

        VK_HANDLE_EXCEPT(vkBeginCommandBuffer(buffer, &beginInfo));
        rn->currentCommand = buffer;
    }

    void EndCommandBuffer(VKRendererData* rn)
    {
        if (rn->currentCommand != VK_NULL_HANDLE)
        {
            VK_HANDLE_EXCEPT(vkEndCommandBuffer(rn->currentCommand));
            rn->currentCommand = VK_NULL_HANDLE;
        }
    }

    void SubmitCommands(VKRendererData* rn)
    {
        constexpr VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        VkSubmitInfo submitInfo
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &rn->isImageAvailable,
            .pWaitDstStageMask = waitStages,
            .commandBufferCount = (uint32_t)rn->commandPool.buffers.size(),
            .pCommandBuffers = rn->commandPool.buffers.data(),
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &rn->isRenderingFinished,
        };

        VK_HANDLE_EXCEPT(vkQueueSubmit(GRAPHICS_DATA.defaultDevice->logicalDevice.graphicsQueue,
            1, &submitInfo, GRAPHICS_DATA.defaultDevice->isInFlight));
    }
}