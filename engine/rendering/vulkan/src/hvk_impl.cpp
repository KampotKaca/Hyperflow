#include "hvk_graphics.h"
#include "hvk_indexbuffer.h"
#include "hvk_renderer.h"
#include "hvk_shader.h"
#include "hvk_texturepack.h"
#include "hvk_vertbuffer.h"
#include "hvk_storagebuffer.h"
#include "hvk_renderpass.h"
#include "hvk_renderpass.h"

namespace hf::inter::rendering
{
    void Load(const RendererLoadInfo& info)
    {
        GRAPHICS_DATA.platform.instance = info.platformInstance;
        GRAPHICS_DATA.platform.platformDll = info.platformDll;
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
        return new VkRenderer(info);
    }

    void PostInstanceLoad(void* rn, RenderPass pass)
    {
        PostRendererLoad((VkRenderer*)rn, pass);
    }

    void DestroyInstance(void* rn)
    {
        delete (VkRenderer*)rn;
    }

    RenderPass DefineRenderPass(const RenderPassDefinitionInfo& info)
    {
        GRAPHICS_DATA.renderPasses.emplace_back(info);
        return GRAPHICS_DATA.renderPasses.size();
    }

    void BindRenderPass(void* rn, RenderPass pass)
    {
        auto rend = (VkRenderer*)rn;
        BindPassToRenderer(rend, pass, rend->targetSize);
    }

    void BeginRenderPass(void* rn, RenderPass pass)
    {
        BeginPass((VkRenderer*)rn, pass);
    }

    void EndRenderPass(void* rn)
    {
        EndPass((VkRenderer*)rn);
    }

    void* CreateShader(const ShaderCreationInfo& info)
    {
        return new VkShader(info);
    }

    void DestroyShader(void* shader)
    {
        delete (VkShader*)shader;
    }

    void BindShader(const void* rn, const void* shader, BufferAttrib attrib)
    {
        BindShader((VkRenderer*)rn, (VkShader*)shader, attrib);
    }

    void* CreateTexture(const TextureCreationInfo& info)
    {
        return new VkTexture(info);
    }

    void DestroyTexture(void* tex)
    {
        delete (VkTexture*)tex;
    }

    void* CreateTexturePack(const TexturePackCreationInfo& info)
    {
        return new VkTexturePack(info);
    }

    void DestroyTexturePack(void* txPack)
    {
        delete (VkTexturePack*)txPack;
    }

    void UploadTexturePack(void* texPack, const TexturePackUploadInfo& info)
    {
        SetTextureBinding((VkTexturePack*)texPack, info.bindingIndex,
            info.sampler, (VkTexture**)info.pTextures, info.textureOffset, info.textureCount);
    }

    void BindTexturePack(void* rn, void* texPack)
    {
        hf::BindTexturePack((VkRenderer*)rn, (VkTexturePack*)texPack);
    }

    void* CreateTexturePackAllocator(const TexturePackAllocatorCreationInfo& info)
    {
        return new VkTexturePackAllocator(info);
    }

    void DestroyTexturePackAllocator(void* texPackAllocator)
    {
        delete (VkTexturePackAllocator*)texPackAllocator;
    }

    TextureSampler DefineTextureSampler(const TextureSamplerDefinitionInfo& info)
    {
        GRAPHICS_DATA.textureSamplers.emplace_back(info);
        return GRAPHICS_DATA.textureSamplers.size();
    }

    TextureLayout DefineTextureLayout(const TextureLayoutDefinitionInfo& info)
    {
        GRAPHICS_DATA.textureLayouts.emplace_back(info);
        return GRAPHICS_DATA.textureLayouts.size();
    }

    BufferAttrib DefineVertBufferAttrib(const BufferAttribDefinitionInfo& info, uint32_t fullStride)
    {
        GRAPHICS_DATA.bufferAttribs.emplace_back(info, fullStride);
        return GRAPHICS_DATA.bufferAttribs.size();
    }

    uint32_t GetVertBufferAttribSize(BufferAttrib attrib)
    {
        auto& attribute = GetAttrib(attrib);
        return attribute.vertexSize;
    }

    UniformBuffer DefineUniformBuffer(const UniformBufferDefinitionInfo& info)
    {
        GRAPHICS_DATA.uniformBuffers.emplace_back(info);
        return GRAPHICS_DATA.uniformBuffers.size();
    }

    void UploadUniformBuffer(const void* rn, const UniformBufferUploadInfo& info)
    {
        UploadUniforms((VkRenderer*)rn, info);
    }

    ShaderSetup DefineShaderSetup(const ShaderSetupDefinitionInfo& info)
    {
        GRAPHICS_DATA.shaderSetups.emplace_back(info);
        return GRAPHICS_DATA.shaderSetups.size();
    }

    void BindShaderSetup(void* rn, ShaderSetup storage)
    {
        hf::BindShaderSetup((VkRenderer*)rn, storage);
    }

    UniformAllocator DefineUniformAllocator(const UniformAllocatorDefinitionInfo& info)
    {
        GRAPHICS_DATA.uniformAllocators.emplace_back(info);
        return GRAPHICS_DATA.uniformAllocators.size();
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

    void* CreateStorageBuffer(const StorageBufferCreationInfo& info)
    {
        return new VkStorageBuffer(info);
    }

    void DestroyStorageBuffer(void* handle)
    {
        delete (VkStorageBuffer*)handle;
    }

    void UploadStorageBuffer(const StorageBufferUploadInfo& info)
    {
        auto buffer = (VkStorageBuffer*)info.storage;
        if (buffer->memoryType == BufferMemoryType::Static)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot modify static buffer");

        if (buffer->mapping) UploadBufferMemory(info.data, buffer->mapping, info.offsetInBytes, info.sizeInBytes);
        else UploadBufferMemory(buffer->bufferMemory, info.data, info.offsetInBytes, info.sizeInBytes);
    }

    void SubmitBufferCopyOperations()
    {
        SubmitBufferToBufferCopyOperations();
    }

    void SubmitTextureCopyOperations()
    {
        SubmitBufferToImageCopyOperations();
    }

    bool GetReadyForRendering(void* rn)
    {
        auto renderer = (VkRenderer*)rn;
        return GetReadyForRendering(renderer);
    }

    void StartFrame(void* rn)
    {
        auto renderer = (VkRenderer*)rn;
        StartFrame(renderer);
    }

    void EndFrame(void* rn)
    {
        auto renderer = (VkRenderer*)rn;
        EndFrame(renderer);
    }

    void Draw(void* rn, const DrawCallInfo& info)
    {
        auto* vrn = (VkRenderer*)rn;

        uint32_t offset = 0;
        uint32_t vertCount = 0;
        for (uint32_t i = 0; i < info.bufferCount; i++)
        {
            auto vertBuffer = (VkVertBuffer*)info.pVertBuffers[i];
            vrn->vertBufferCache[i] = vertBuffer->buffer;
            vrn->drawOffsets[i] = offset;
            offset += GetAttrib(vertBuffer->attrib).vertexSize;
            vertCount = vertBuffer->vertCount;
        }

        VkDrawInfo drawInfo
        {
            .renderer = vrn,
            .pBuffers = vrn->vertBufferCache,
            .pOffsets = vrn->drawOffsets,
            .bufferCount = info.bufferCount,
            .vertCount = vertCount,
            .instanceCount = info.instanceCount,
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

    void RegisterFrameBufferChange(void* rn, uvec2 newSize)
    {
        auto renderer = (VkRenderer*)rn;
        RegisterFrameBufferChange(renderer, newSize);
    }

    void SetVSync(void* rn, bool isOn)
    {
        SetVSync((VkRenderer*)rn, isOn);
    }

    API RendererAPI* GetAPI()
    {
        static RendererAPI api =
        {
            //loading
            .Load                       = Load,
            .Unload                     = Unload,
            .CreateInstance             = CreateInstance,
            .PostInstanceLoad           = PostInstanceLoad,
            .DestroyInstance            = DestroyInstance,

            //draw pass
            .DefineRenderPass           = DefineRenderPass,
            .BindRenderPass             = BindRenderPass,
            .BeginRenderPass            = BeginRenderPass,
            .EndRenderPass              = EndRenderPass,

            //shader
            .CreateShader               = CreateShader,
            .DestroyShader              = DestroyShader,
            .BindShader                 = BindShader,

            //uniform storage
            .DefineShaderSetup          = DefineShaderSetup,
            .BindShaderSetup            = BindShaderSetup,

            //texture
            .CreateTexture              = CreateTexture,
            .DestroyTexture             = DestroyTexture,

            //texture pack
            .CreateTexturePack          = CreateTexturePack,
            .DestroyTexturePack         = DestroyTexturePack,
            .UploadTexturePack          = UploadTexturePack,
            .BindTexturePack            = BindTexturePack,

            //texture pack allocator
            .CreateTexturePackAllocator     = CreateTexturePackAllocator,
            .DestroyTexturePackAllocator    = DestroyTexturePackAllocator,

            //texture sampler
            .DefineTextureSampler       = DefineTextureSampler,

            //texture layout
            .DefineTextureLayout        = DefineTextureLayout,

            //buffer attribute
            .DefineVertBufferAttrib     = DefineVertBufferAttrib,
            .GetVertBufferAttribSize    = GetVertBufferAttribSize,

            //uniform buffer
            .DefineUniformBuffer        = DefineUniformBuffer,
            .UploadUniformBuffer        = UploadUniformBuffer,

            //uniform allocator
            .DefineUniformAllocator     = DefineUniformAllocator,

            //vertex buffer
            .CreateVertBuffer           = CreateVertBuffer,
            .DestroyVertBuffer          = DestroyVertBuffer,
            .UploadVertBuffer           = UploadVertBuffer,

            //index buffer
            .CreateIndexBuffer          = CreateIndexBuffer,
            .DestroyIndexBuffer         = DestroyIndexBuffer,
            .UploadIndexBuffer          = UploadIndexBuffer,

            //storage buffer
            .CreateStorageBuffer        = CreateStorageBuffer,
            .DestroyStorageBuffer       = DestroyStorageBuffer,
            .UploadStorageBuffer        = UploadStorageBuffer,

            //buffer operations
            .SubmitBufferCopyOperations   = SubmitBufferCopyOperations,
            .SubmitTextureCopyOperations  = SubmitTextureCopyOperations,

            //rendering
            .GetReadyForRendering       = GetReadyForRendering,
            .StartFrame                 = StartFrame,
            .EndFrame                   = EndFrame,
            .Draw                       = Draw,
            .WaitForRendering           = WaitForRendering,

            .RegisterFrameBufferChange  = RegisterFrameBufferChange,
            .SetVSync                   = SetVSync,
        };
        return &api;
    }
}
