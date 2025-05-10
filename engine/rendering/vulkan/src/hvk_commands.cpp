#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    void CreateCommandPool(const GraphicsDevice& device, uint32_t familyIndex, CommandPool* result)
    {
        VkCommandPoolCreateInfo poolCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = familyIndex
        };

        VK_HANDLE_EXCEPT(vkCreateCommandPool(device.logicalDevice.device, &poolCreateInfo, nullptr, &result->pool));
    }

    void DestroyCommandPool(const GraphicsDevice& device, CommandPool& pool)
    {
        vkDestroyCommandPool(device.logicalDevice.device, pool.pool, nullptr);
        pool.pool = VK_NULL_HANDLE;
    }

    void CreateCommandBuffers(const GraphicsDevice& device, CommandPool* pool, const uint32_t count)
    {
        VkCommandBufferAllocateInfo allocInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = pool->pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = count,
        };

        std::vector<VkCommandBuffer> commandBuffers(count);
        VK_HANDLE_EXCEPT(vkAllocateCommandBuffers(device.logicalDevice.device, &allocInfo, commandBuffers.data()));
        for (uint32_t i = 0; i < count; i++) pool->buffers.push_back(commandBuffers[i]);
    }

    void BeginCommandBuffer(VkRenderer* rn, VkCommandBuffer buffer)
    {
        auto& frame = rn->frames[rn->currentFrame];
        if (frame.usedCommandCount >= VULKAN_API_MAX_COMMANDS_PER_FRAME)
            throw GENERIC_EXCEPT("[Vulkan]", "Please increase MAX_COMMANDS_PER_FRAME");

        VkCommandBufferBeginInfo beginInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        };

        VK_HANDLE_EXCEPT(vkResetCommandBuffer(buffer, 0));
        VK_HANDLE_EXCEPT(vkBeginCommandBuffer(buffer, &beginInfo));
        rn->currentCommand = buffer;
    }

    void EndCommandBuffer(VkRenderer* rn)
    {
        if (rn->currentCommand != VK_NULL_HANDLE)
        {
            auto& frame = rn->frames[rn->currentFrame];
            frame.usedCommands[frame.usedCommandCount] = rn->currentCommand;
            frame.usedCommandCount++;
            VK_HANDLE_EXCEPT(vkEndCommandBuffer(rn->currentCommand));
            rn->currentCommand = VK_NULL_HANDLE;
        }
    }

    void SubmitCommands(VkRenderer* rn)
    {
        constexpr VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        auto& frame = rn->frames[rn->currentFrame];

        VkSubmitInfo submitInfo
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &frame.isImageAvailable,
            .pWaitDstStageMask = waitStages,
            .commandBufferCount = frame.usedCommandCount,
            .pCommandBuffers = frame.usedCommands,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &frame.isRenderingFinished,
        };

        VK_HANDLE_EXCEPT(vkQueueSubmit(GRAPHICS_DATA.defaultDevice->logicalDevice.graphicsQueue,
            1, &submitInfo, frame.isInFlight));
    }
}
