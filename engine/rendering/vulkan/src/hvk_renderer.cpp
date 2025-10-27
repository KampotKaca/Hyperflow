#include "hvk_renderer.h"
#include "hvk_graphics.h"

namespace hf
{
    void DestroySurface(VkRenderer* rn)
    {
        vkDestroySurfaceKHR(GRAPHICS_DATA.instance, rn->swapchain.surface, &GRAPHICS_DATA.platform.allocator);
        rn->swapchain.surface = VK_NULL_HANDLE;
    }

    uvec2 GetReadyForRendering(VkRenderer* rn, VkRenderTexture** pTextures, uint32_t textureCount)
    {
        if (rn->targetSize.x == 0 || rn->targetSize.y == 0) return {};
        return AcquireNextImage(rn, pTextures, textureCount);
    }

    void StartFrame(VkRenderer* rn)
    {
        auto& frame = rn->frames[rn->currentFrame];
        frame.usedCommandCount = 0;
        BeginCommandBuffer(rn, rn->commandPool.buffers[rn->currentFrame]);
    }

    void EndFrame(VkRenderer* rn)
    {
        EndCommandBuffer(rn);
        SubmitCommands(rn);
        PresentSwapchain(rn);

        rn->currentFrame = (rn->currentFrame + 1) % rn->frames.size();
    }

    void RegisterFrameBufferChange(VkRenderer* rn, uvec2 newSize)
    {
        std::lock_guard lock(rn->frameSync);
        rn->targetSize = newSize;
        rn->frameBufferChanged = true;
    }

    void SetVSync(VkRenderer* rn, VsyncMode mode)
    {
        std::lock_guard lock(rn->frameSync);
        rn->vSyncMode = mode;
        rn->frameBufferChanged = true;
    }

    struct DrawStartData
    {
        uint32_t bufferCount;
        uint32_t instanceCount;
    };

    static DrawStartData DrawStart(const VkRenderer* rn, const ir::rdr::DrawCallInfo_i& info,
                                   VkBuffer* vbCache, VkDeviceSize* offsetsCache)
    {
        uint32_t bufferCount = info.vertexBufferCount;
        uint32_t instanceCount = 1;

        for (uint32_t i = 0; i < info.vertexBufferCount; i++)
        {
            auto& vb = info.pVertexBuffers[i];
            const auto vertexBuffer = (VkVertexBuffer*)vb.buffer;
            vbCache[i] = vertexBuffer->buffers[rn->currentFrame];
            offsetsCache[i] = vb.offsetInBytes;
        }

        if (info.instanceBuffer.has_value())
        {
            const auto instanceBuffer = info.instanceBuffer.value();
            vbCache[bufferCount] = ((VkVertexBuffer*)instanceBuffer.buffer)->buffers[rn->currentFrame];
            offsetsCache[bufferCount] = instanceBuffer.offsetInBytes;
            bufferCount++;
            instanceCount = instanceBuffer.instanceCount;
        }

        return DrawStartData{ bufferCount, instanceCount };
    }

    void Draw(const VkRenderer* rn, const ir::rdr::IndexedDrawCallInfo_i& info)
    {
        VkBuffer vertexBufferCache[MAX_NUM_BUFFER_CACHE]{};
        VkDeviceSize drawOffsets[MAX_NUM_BUFFER_CACHE]{};

        const auto data = DrawStart(rn, info.drawInfo, vertexBufferCache, drawOffsets);

        const auto command = rn->currentCommand;
        vkCmdBindVertexBuffers(command, 0, data.bufferCount, vertexBufferCache, drawOffsets);
        const auto indexBuffer = (VkIndexBuffer*)info.indexBuffer.buffer;
        vkCmdBindIndexBuffer(command, indexBuffer->buffers[rn->currentFrame], info.indexBuffer.offset * (uint64_t)BUFFER_DATA_SIZE[(uint32_t)indexBuffer->indexType], indexBuffer->indexType);

        vkCmdDrawIndexed(command, info.indexBuffer.indexCount, data.instanceCount, 0, 0, 0);
    }

    void Draw(const VkRenderer* rn, const ir::rdr::VertexedDrawCallInfo_i& info)
    {
        VkBuffer vertexBufferCache[MAX_NUM_BUFFER_CACHE]{};
        VkDeviceSize drawOffsets[MAX_NUM_BUFFER_CACHE]{};

        const auto data = DrawStart(rn, info.drawInfo, vertexBufferCache, drawOffsets);

        const auto command = rn->currentCommand;
        vkCmdBindVertexBuffers(command, 0, data.bufferCount, vertexBufferCache, drawOffsets);
        vkCmdDraw(command, info.vertexCount, data.instanceCount, 0, 0);
    }
}
