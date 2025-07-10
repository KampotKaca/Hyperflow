#include "hvk_graphics.h"
#include "hvk_indexbuffer.h"
#include "hvk_renderer.h"
#include "hvk_shader.h"
#include "hvk_texturepack.h"
#include "hvk_vertbuffer.h"
#include "hvk_storagebuffer.h"
#include "hvk_rendertexture.h"

namespace hf::inter::rendering
{
    void Load(const RendererLoadInfo& info)
    {
        GRAPHICS_DATA.platform.createVulkanSurfaceFunc = info.createVulkanSurfaceFunc;
        GRAPHICS_DATA.platform.platformDll = info.platformDll;
        GRAPHICS_DATA.platform.api = GetAPI();
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

    void DestroyInstance(void* rn)
    {
        delete (VkRenderer*)rn;
    }

    void* CreateShader(const ShaderCreationInfo& info)
    {
        return new VkShader(info);
    }

    void DestroyShader(void* shader)
    {
        delete (VkShader*)shader;
    }

    void BindShader(const void* rn, const ShaderBindingInfo& info)
    {
        BindShader((VkRenderer*)rn, (VkShader*)info.shader, info.attrib, info.bindingPoint);
    }

    void* CreateTexture(const TextureCreationInfo& info)
    {
        return new VkTexture(info);
    }

    void DestroyTexture(void* tex)
    {
        delete (VkTexture*)tex;
    }

    void* CreateRenderTexture(const RenderTextureCreationInfo& info)
    {
        return new VkRenderTexture(info);
    }

    void DestroyRenderTexture(void* tex)
    {
        delete (VkRenderTexture*)tex;
    }

    void* CreateTexturePack(const TexturePackCreationInfo& info)
    {
        return new VkTexturePack(info);
    }

    void DestroyTexturePack(void* txPack)
    {
        delete (VkTexturePack*)txPack;
    }

    void UploadTexturePackBinding(void* texPack, const TexturePackBindingUploadGroupInfo& info)
    {
        SetTextureBinding((VkTexturePack*)texPack, info.bindings, info.bindingCount);
    }

    void BindTexturePack(void* rn, const TexturePackBindingInfo& info)
    {
        hf::BindTexturePack((VkRenderer*)rn, (VkTexturePack*)info.texturePack, info.setBindingIndex);
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
        GRAPHICS_DATA.textureSamplers.emplace_back(MakeURef<VkTextureSampler>(info));
        return GRAPHICS_DATA.textureSamplers.size();
    }

    TextureLayout DefineTextureLayout(const TextureLayoutDefinitionInfo& info)
    {
        GRAPHICS_DATA.textureLayouts.emplace_back(MakeURef<VkTextureLayout>(info));
        return GRAPHICS_DATA.textureLayouts.size();
    }

    BufferAttrib DefineVertBufferAttrib(const BufferAttribDefinitionInfo& info, uint32_t fullStride)
    {
        GRAPHICS_DATA.bufferAttribs.emplace_back(MakeURef<VkBufferAttrib>(info, fullStride));
        return GRAPHICS_DATA.bufferAttribs.size();
    }

    uint32_t GetVertBufferAttribSize(BufferAttrib attrib)
    {
        auto& attribute = GetAttrib(attrib);
        return attribute->vertexSize;
    }

    Buffer DefineUniformBuffer(const BufferDefinitionInfo& info)
    {
        GRAPHICS_DATA.buffers.emplace_back(MakeURef<VkUniformBuffer>(info));
        return GRAPHICS_DATA.buffers.size();
    }

    Buffer DefineStorageBuffer(const StorageBufferDefinitionInfo& info)
    {
        GRAPHICS_DATA.buffers.emplace_back(MakeURef<VkStorageBuffer>(info));
        return GRAPHICS_DATA.buffers.size();
    }

    void UploadBuffer(const void* rn, const BufferUploadInfo& info)
    {
        UploadBuffers((VkRenderer*)rn, info);
    }

    void BindBuffer(const void* rn, const BufferBindInfo& info)
    {
        BindBuffers((VkRenderer*)rn, info);
    }

    BufferAllocator DefineBufferAllocator(const BufferAllocatorDefinitionInfo& info)
    {
        GRAPHICS_DATA.bufferAllocators.emplace_back(MakeURef<VkBufferAllocator>(info));
        return GRAPHICS_DATA.bufferAllocators.size();
    }

    ShaderSetup DefineShaderSetup(const ShaderSetupDefinitionInfo& info)
    {
        GRAPHICS_DATA.shaderSetups.emplace_back(MakeURef<VkShaderSetup>(info));
        return GRAPHICS_DATA.shaderSetups.size();
    }

    void BindShaderSetup(void* rn, ShaderSetup setup)
    {
        hf::BindShaderSetup((VkRenderer*)rn, setup);
    }

    void UploadPushConstants(void* rn, const PushConstantUploadInfo& info)
    {
        hf::UploadPushConstants((VkRenderer*)rn, info);
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
        auto fullSize = (uint64_t)attribute->vertexSize * info.vertexCount;
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

    void SubmitBufferCopyOperations()
    {
        SubmitBufferToBufferCopyOperations();
    }

    void SubmitTextureCopyOperations()
    {
        SubmitBufferToImageCopyOperations();
    }

    uvec2 GetReadyForRendering(void* rn, void** pTextures, uint32_t textureCount)
    {
        auto renderer = (VkRenderer*)rn;
        return GetReadyForRendering(renderer, (VkRenderTexture**)pTextures, textureCount);
    }

    void WaitForPreviousFrame(void* rn)
    {
        DelayUntilPreviousFrameFinish((VkRenderer*)rn);
    }

    void StartFrame(void* rn, uint32_t renderTextureCount)
    {
        auto renderer = (VkRenderer*)rn;
        renderer->currentRenderTextureCount = 0;
        renderer->targetRenderTextureCount = renderTextureCount;
        StartFrame(renderer);
    }

    void EndFrame(void* rn)
    {
        auto renderer = (VkRenderer*)rn;
        renderer->prevRenderTexture = nullptr;
        renderer->currentRenderTexture = nullptr;
        renderer->currentRenderTextureCount = 0;
        renderer->targetRenderTextureCount = 0;
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
            offset += GetAttrib(vertBuffer->attrib)->vertexSize;
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

    void ApplyRenderAttachmentDependencies(void* rn, RenderAttachmentDependencyInfo* pInfos, uint32_t count)
    {
        static VkImageMemoryBarrier2 barriers[RENDERING_MAX_NUM_RENDER_ATTACHMENT_DEPENDENCIES]{};
        auto* vrn = (VkRenderer*)rn;

        for (uint32_t i = 0; i < count; i++)
        {
            auto& info = pInfos[i];
            barriers[i] =
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                .srcStageMask = (VkPipelineStageFlags2)info.src.stageMask,
                .srcAccessMask = (VkAccessFlags2)info.src.accessMask,
                .dstStageMask = (VkPipelineStageFlags2)info.dst.stageMask,
                .dstAccessMask = (VkAccessFlags2)info.dst.accessMask,
                .oldLayout = (VkImageLayout)info.src.targetLayout,
                .newLayout = (VkImageLayout)info.dst.targetLayout,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = GetRenderTextureImage(vrn->currentRenderTexture, info.attachmentIndex),
                .subresourceRange =
                {
                    .aspectMask = (VkImageAspectFlags)info.aspectFlags,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
            };
        }

        VkDependencyInfo depInfo
        {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = count,
            .pImageMemoryBarriers = barriers,
        };

        GRAPHICS_DATA.extensionFunctions.vkCmdPipelineBarrier2KHR(vrn->currentCommand, &depInfo);
    }

    void WaitForDevice()
    {
        hf::WaitForDevice();
    }

    void BeginRendering(void* rn, void* tex)
    {
        auto vrn = (VkRenderer*)rn;

        if (vrn->currentRenderTexture)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot begin rendering while rendering is in progress");

        vrn->currentRenderTexture = (VkRenderTexture*)tex;
        hf::BeginRendering(vrn);
        vrn->currentRenderTextureCount++;
    }

    void EndRendering(void* rn)
    {
        auto vrn = (VkRenderer*)rn;

        if (!vrn->currentRenderTexture)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot end rendering if rendering is not in progress");

        vrn->prevRenderTexture = vrn->currentRenderTexture;
        vrn->currentRenderTexture = nullptr;
        hf::EndRendering(vrn);
    }

    void RegisterFrameBufferChange(void* rn, uvec2 newSize)
    {
        auto renderer = (VkRenderer*)rn;
        RegisterFrameBufferChange(renderer, newSize);
    }

    void SetVSync(void* rn, VsyncMode mode)
    {
        SetVSync((VkRenderer*)rn, mode);
    }

    void* GetCmd(void* rn)
    {
        return ((VkRenderer*)rn)->currentCommand;
    }

    void* GetEditorInfo()
    {
        return LoadEditorInfo();
    }

    API RendererAPI* GetAPI()
    {
        static RendererAPI api =
        {
            //loading
            .Load                       = Load,
            .Unload                     = Unload,
            .CreateInstance             = CreateInstance,
            .DestroyInstance            = DestroyInstance,

            //shader
            .CreateShader               = CreateShader,
            .DestroyShader              = DestroyShader,
            .BindShader                 = BindShader,

            //uniform storage
            .DefineShaderSetup          = DefineShaderSetup,
            .BindShaderSetup            = BindShaderSetup,
            .UploadPushConstants        = UploadPushConstants,

            //texture
            .CreateTexture              = CreateTexture,
            .DestroyTexture             = DestroyTexture,

            //render texture
            .CreateRenderTexture        = CreateRenderTexture,
            .DestroyRenderTexture       = DestroyRenderTexture,

            //texture pack
            .CreateTexturePack          = CreateTexturePack,
            .DestroyTexturePack         = DestroyTexturePack,
            .UploadTexturePackBinding   = UploadTexturePackBinding,
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

            //buffers
            .DefineUniformBuffer        = DefineUniformBuffer,
            .DefineStorageBuffer        = DefineStorageBuffer,
            .UploadBuffer               = UploadBuffer,
            .BindBuffer                 = BindBuffer,

            //uniform allocator
            .DefineBufferAllocator     = DefineBufferAllocator,

            //vertex buffer
            .CreateVertBuffer           = CreateVertBuffer,
            .DestroyVertBuffer          = DestroyVertBuffer,
            .UploadVertBuffer           = UploadVertBuffer,

            //index buffer
            .CreateIndexBuffer          = CreateIndexBuffer,
            .DestroyIndexBuffer         = DestroyIndexBuffer,
            .UploadIndexBuffer          = UploadIndexBuffer,

            //buffer operations
            .SubmitBufferCopyOperations   = SubmitBufferCopyOperations,
            .SubmitTextureCopyOperations  = SubmitTextureCopyOperations,

            //rendering
            .WaitForPreviousFrame       = WaitForPreviousFrame,
            .GetReadyForRendering       = GetReadyForRendering,
            .StartFrame                 = StartFrame,
            .EndFrame                   = EndFrame,
            .Draw                       = Draw,
            .ApplyRenderAttachmentDependencies = ApplyRenderAttachmentDependencies,
            .WaitForDevice              = WaitForDevice,

            .BeginRendering             = BeginRendering,
            .EndRendering               = EndRendering,

            .RegisterFrameBufferChange  = RegisterFrameBufferChange,
            .SetVSync                   = SetVSync,
            .GetCmd                     = GetCmd,

            .GetEditorInfo              = GetEditorInfo,
        };
        return &api;
    }
}
