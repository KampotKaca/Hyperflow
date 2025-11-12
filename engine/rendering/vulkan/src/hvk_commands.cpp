#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    void CreateCommandPool(const GraphicsDevice& device, uint32_t familyIndex, CommandPool* result)
    {
        VkCommandPoolCreateInfo poolCreateInfo{};
        poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolCreateInfo.queueFamilyIndex = familyIndex;

        hvk_assert(vkCreateCommandPool(device.logicalDevice.device, &poolCreateInfo, &GRAPHICS_DATA.platform.allocator, &result->pool), "vkCreateCommandPool Failed!");
    }

    void DestroyCommandPool(const GraphicsDevice& device, CommandPool& pool)
    {
        vkDestroyCommandPool(device.logicalDevice.device, pool.pool, &GRAPHICS_DATA.platform.allocator);
        pool.pool = VK_NULL_HANDLE;
    }

    void CreateCommandBuffers(const GraphicsDevice& device, CommandPool* pool, const uint32_t count)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = pool->pool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = count;

        List<VkCommandBuffer> commandBuffers(count);
        hvk_assert(vkAllocateCommandBuffers(device.logicalDevice.device, &allocInfo, commandBuffers.data()), "vkAllocateCommandBuffers Failed!");
        for (uint32_t i = 0; i < count; i++) pool->buffers.push_back(commandBuffers[i]);
    }

    void BeginCommandBuffer(VkRenderer* rn, VkCommandBuffer buffer)
    {
        auto& frame = rn->frames[rn->currentFrame];
        hassert(frame.usedCommandCount < VULKAN_API_MAX_COMMANDS_PER_FRAME, "[Vulkan] Please increase MAX_COMMANDS_PER_FRAME")

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        hvk_assert(vkResetCommandBuffer(buffer, 0), "vkResetCommandBuffer Failed!");
        hvk_assert(vkBeginCommandBuffer(buffer, &beginInfo), "vkBeginCommandBuffer Failed!");
        rn->currentCommand = buffer;
    }

    void EndCommandBuffer(VkRenderer* rn)
    {
        if (rn->currentCommand != VK_NULL_HANDLE)
        {
            auto& frame = rn->frames[rn->currentFrame];
            frame.usedCommands[frame.usedCommandCount] = rn->currentCommand;
            frame.usedCommandCount++;
            hvk_assert(vkEndCommandBuffer(rn->currentCommand), "vkEndCommandBuffer Failed!");
            rn->currentCommand = VK_NULL_HANDLE;
        }
    }

    void SubmitCommands(VkRenderer* rn)
    {
        constexpr VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        const auto& frame = rn->frames[rn->currentFrame];
        const auto& image = rn->swapchain.images[rn->imageIndex];

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &frame.isImageAvailable;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = frame.usedCommandCount;
        submitInfo.pCommandBuffers = frame.usedCommands;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &image.isRenderingFinished;

        const auto& device = GRAPHICS_DATA.device.logicalDevice;
        hvk_assert(vkResetFences(device.device, 1, &image.isInFlight), "vkResetFences Failed!");
        hvk_assert(vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, image.isInFlight), "vkQueueSubmit Failed!");
    }
}
