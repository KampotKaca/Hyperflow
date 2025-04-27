#include "hvk_graphics.h"
#include "hvk_renderer.h"
#include "hvk_shader.h"
#include "hvk_vertbuffer.h"

namespace hf::inter::rendering
{
    void Load(const RendererLoadInfo& info)
    {
        LoadVulkan(info);
    }

    void Unload()
    {
        UnloadVulkan();
        GRAPHICS_DATA = {};
    }

    void* CreateInstance(const RendererInstanceCreationInfo& info)
    {
        return new VKRenderer(info.handle, info.size);
    }

    void DestroyInstance(void* rnInstance)
    {
        delete((VKRenderer*)rnInstance);
    }

    void* CreateShader(const ShaderCreationInfo& info)
    {
        return new VkShader(info);
    }

    void DestroyShader(void* shader)
    {
        delete (VkShader*)shader;
    }

    void BindShader(const void* renderer, const void* shader, BufferAttrib attrib)
    {
        BindShader((VKRenderer*)renderer, (VkShader*)shader, attrib);
    }

    uint32_t CreateBufferAttrib(const BufferAttribCreateInfo& info, uint32_t fullStride)
    {
        GRAPHICS_DATA.bufferAttribs.emplace_back(info, fullStride);
        return GRAPHICS_DATA.bufferAttribs.size();
    }

    void* CreateVertBuffer(const VertBufferCreationInfo& info)
    {
        return new VkVertBuffer(info);
    }

    void DestroyVertBuffer(void* handle)
    {
        delete (VkVertBuffer*)handle;
    }

    void UploadVertBuffer(const VertBufferUploadInfo& info)
    {
        auto buffer = (VkVertBuffer*)info.buffer;
        if (buffer->memoryType == VertBufferMemoryType::Static)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot modify static buffer");

        auto& attribute = GetAttrib(buffer->attrib);
        auto fullSize = attribute.vertexSize * info.vertexCount;
        auto fullOffset = info.offset * info.vertexCount;

        UploadBufferMemory(buffer->bufferMemory, info.data, fullOffset, fullSize);
    }

    void SubmitStagedCopyOperations()
    {
        hf::SubmitStagedCopyOperations();
    }

    bool GetReadyForRendering(void* rn)
    {
        auto renderer = (VKRenderer*)rn;
        return GetReadyForRendering(renderer);
    }

    void StartFrame(void* rn)
    {
        auto renderer = (VKRenderer*)rn;
        StartFrame(renderer);
    }

    void EndFrame(void* rn)
    {
        auto renderer = (VKRenderer*)rn;
        EndFrame(renderer);
    }

    void RegisterFrameBufferChange(void* rn, uvec2 newSize)
    {
        auto renderer = (VKRenderer*)rn;
        RegisterFrameBufferChange(renderer, newSize);
    }

    void Draw(const DrawCallInfo& info)
    {
        auto* rn = (VKRenderer*)info.renderer;

        uint32_t offset = 0;
        uint32_t vertCount = 0;
        for (uint32_t i = 0; i < info.bufferCount; i++)
        {
            auto buffer = (VkVertBuffer*)info.pBuffers[i];
            rn->drawBuffers[i] = buffer->buffer;
            rn->drawOffsets[i] = offset;
            offset += GetAttrib(buffer->attrib).vertexSize;
            vertCount = buffer->vertCount;
        }

        VkDrawInfo drawInfo
        {
            .renderer = rn,
            .bufferCount = info.bufferCount,
            .pBuffers = rn->drawBuffers,
            .pOffsets = rn->drawOffsets,
            .vertCount = vertCount,
            .instanceCount = info.instanceCount
        };

        Draw(drawInfo);
    }

    void WaitForRendering()
    {
        DelayThreadUntilRenderingFinish();
    }

    API RendererAPI* GetAPI()
    {
        static RendererAPI api =
        {
            &Load,
            &Unload,
            &CreateInstance,
            &DestroyInstance,
            &CreateShader,
            &DestroyShader,
            &BindShader,
            &CreateBufferAttrib,
            &CreateVertBuffer,
            &DestroyVertBuffer,
            &UploadVertBuffer,
            &SubmitStagedCopyOperations,
            &GetReadyForRendering,
            &StartFrame,
            &EndFrame,
            &RegisterFrameBufferChange,
            &Draw,
            &WaitForRendering
        };
        return &api;
    }
}