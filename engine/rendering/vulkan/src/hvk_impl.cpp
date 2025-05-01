#include "hvk_graphics.h"
#include "hvk_indexbuffer.h"
#include "hvk_renderer.h"
#include "hvk_shader.h"
#include "hvk_vertbuffer.h"

namespace hf::inter::rendering
{
    void Load(const RendererLoadInfo& info)
    {
        GRAPHICS_DATA.platform.instance = info.platformInstance;
        GRAPHICS_DATA.platform.platformDll = info.platformDll;
        GRAPHICS_DATA.rendererPreloadCallback = info.rendererPreloadCallback;
        auto func = (VulkanPlatformAPI*(*)())info.getFuncFromDll(GRAPHICS_DATA.platform.platformDll, "GetAPI");

        if (!func) throw GENERIC_EXCEPT("[Hyperflow]", "Failed to get vulkan platform API");
        GRAPHICS_DATA.platform.api = func();
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

    uint32_t DefineVertBufferAttrib(const BufferAttribDefinitionInfo& info, uint32_t fullStride)
    {
        GRAPHICS_DATA.bufferAttribs.emplace_back(info, fullStride);
        return GRAPHICS_DATA.bufferAttribs.size();
    }

    uint32_t DefineUniformBuffer(const UniformBufferDefinitionInfo& info)
    {
        GRAPHICS_DATA.uniformBuffers.emplace_back(info);
        return GRAPHICS_DATA.uniformBuffers.size();
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
        if (buffer->memoryType == BufferMemoryType::Static)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot modify static buffer");

        auto& attribute = GetAttrib(buffer->attrib);
        auto fullSize = (uint64_t)attribute.vertexSize * info.vertexCount;
        auto fullOffset = (uint64_t)info.offset * info.vertexCount;

        UploadBufferMemory(buffer->bufferMemory, info.data, fullOffset, fullSize);
    }

    void* CreateIndexBuffer(const IndexBufferCreationInfo& info)
    {
        return new VkIndexBuffer(info);
    }

    void DestroyIndexBuffer(void* handle)
    {
        delete (VkIndexBuffer*)handle;
    }

    void UploadIndexBuffer(const IndexBufferUploadInfo& info)
    {
        auto buffer = (VkIndexBuffer*)info.buffer;
        if (buffer->memoryType == BufferMemoryType::Static)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot modify static buffer");

        auto fullSize = (uint64_t)BUFFER_DATA_SIZE[(uint32_t)buffer->indexFormat] * info.indexCount;
        auto fullOffset = (uint64_t)info.offset * info.indexCount;

        UploadBufferMemory(buffer->bufferMemory, info.data, fullOffset, fullSize);
    }

    void SubmitStagedCopyOperations()
    {
        hf::SubmitStagedCopyOperations();
    }

    void DefineUniformBuffer()
    {

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
            auto vertBuffer = (VkVertBuffer*)info.pVertBuffers[i];
            rn->vertBufferCache[i] = vertBuffer->buffer;
            rn->drawOffsets[i] = offset;
            offset += GetAttrib(vertBuffer->attrib).vertexSize;
            vertCount = vertBuffer->vertCount;
        }

        VkDrawInfo drawInfo
        {
            .renderer = rn,
            .pBuffers = rn->vertBufferCache,
            .pOffsets = rn->drawOffsets,
            .bufferCount = info.bufferCount,
            .vertCount = vertCount,
            .instanceCount = info.instanceCount
        };

        if (info.indexBuffer)
        {
            auto indexBuffer = (VkIndexBuffer*)info.indexBuffer;
            drawInfo.indexBuffer = indexBuffer->buffer;
            drawInfo.indexType = indexBuffer->indexType;
            drawInfo.indexCount = indexBuffer->indexCount;
        }

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
            //loading
            .Load                       = &Load,
            .Unload                     = &Unload,
            .CreateInstance             = &CreateInstance,
            .DestroyInstance            = &DestroyInstance,

            //shader
            .CreateShader               = &CreateShader,
            .DestroyShader              = &DestroyShader,
            .BindShader                 = &BindShader,

            //buffer attribute
            .DefineVertBufferAttrib     = &DefineVertBufferAttrib,

            //uniform buffer
            .DefineUniformBuffer        = &DefineUniformBuffer,

            //vertex buffer
            .CreateVertBuffer           = &CreateVertBuffer,
            .DestroyVertBuffer          = &DestroyVertBuffer,
            .UploadVertBuffer           = &UploadVertBuffer,

            //index buffer
            .CreateIndexBuffer          = &CreateIndexBuffer,
            .DestroyIndexBuffer         = &DestroyIndexBuffer,
            .UploadIndexBuffer          = &UploadIndexBuffer,

            //buffer operations
            .SubmitStagedCopyOperations = &SubmitStagedCopyOperations,

            //rendering
            .GetReadyForRendering       = &GetReadyForRendering,
            .StartFrame                 = &StartFrame,
            .EndFrame                   = &EndFrame,
            .RegisterFrameBufferChange  = &RegisterFrameBufferChange,
            .Draw                       = &Draw,
            .WaitForRendering           = &WaitForRendering
        };
        return &api;
    }
}
