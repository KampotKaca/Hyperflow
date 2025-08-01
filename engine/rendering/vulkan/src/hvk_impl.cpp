#include "hvk_graphics.h"
#include "hvk_indexbuffer.h"
#include "hvk_renderer.h"
#include "hvk_shader.h"
#include "hvk_texturepack.h"
#include "hvk_vertbuffer.h"
#include "hvk_storagebuffer.h"
#include "hvk_rendertexture.h"
#include "hvk_shaderlibrary.h"
#include "hvk_boundbuffer.h"

namespace hf::inter::rendering
{
    void Load(const RendererLoadInfo_i& info)
    {
        GRAPHICS_DATA.platform.functions = info.functions;
        GRAPHICS_DATA.platform.platformDll = info.platformDll;
        GRAPHICS_DATA.platform.api = GetAPI();
        LoadVulkan(info);
    }

    void Unload()
    {
        UnloadVulkan();
        GRAPHICS_DATA = {};
    }

    void* CreateInstance(const RendererInstanceCreationInfo_i& info)
    {
        return new VkRenderer(info);
    }

    void DestroyInstance(void* rn)
    {
        delete (VkRenderer*)rn;
    }

    void* CreateShaderLibrary(const ShaderLibraryCreationInfo_i& info)
    {
        return new VkShaderLibrary(info);
    }

    void DestroyShaderLibrary(void* library)
    {
        delete (VkShaderLibrary*)library;
    }

    void* CreateShader(const ShaderCreationInfo_i& info)
    {
        return new VkShader(info);
    }

    void DestroyShader(void* shader)
    {
        delete (VkShader*)shader;
    }

    void BindShader(const void* rn, const void* shader)
    {
        BindShader((VkRenderer*)rn, (VkShader*)shader);
    }

    void* CreateTexture(const TextureCreationInfo_i& info)
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

    void* CreateTexturePack(const TexturePackCreationInfo_i& info)
    {
        return new VkTexturePack(info);
    }

    void DestroyTexturePack(void* txPack)
    {
        delete (VkTexturePack*)txPack;
    }

    void UploadTexturePackBinding(void* texPack, const TexturePackBindingUploadGroupInfo_i& info)
    {
        SetTextureBinding((VkTexturePack*)texPack, info.bindings, info.bindingCount);
    }

    void BindTexturePack(void* rn, const BindResourceInfo_i<void*>& info)
    {
        BindResourceInfo_i<VkTexturePack*> texPackInfo;
        texPackInfo.bindingType = info.bindingType;
        texPackInfo.setBindingIndex = info.setBindingIndex;
        texPackInfo.objectCount = info.objectCount;
        for (uint32_t i = 0; i < info.objectCount; ++i) texPackInfo.objects[i] = (VkTexturePack*)info.objects[i];

        BindTexturePack((VkRenderer*)rn, texPackInfo);
    }

    TextureSampler DefineTextureSampler(const TextureSamplerDefinitionInfo& info)
    {
        GRAPHICS_DATA.textureSamplers.emplace_back(MakeURef<VkTextureSampler>(info));
        return (TextureSampler)GRAPHICS_DATA.textureSamplers.size();
    }

    TextureLayout DefineTextureLayout(const TextureLayoutDefinitionInfo& info)
    {
        GRAPHICS_DATA.textureLayouts.emplace_back(MakeURef<VkTextureLayout>(info));
        return (TextureLayout)GRAPHICS_DATA.textureLayouts.size();
    }

    BufferAttrib DefineVertBufferAttrib(const BufferAttribDefinitionInfo& info, uint32_t fullStride)
    {
        GRAPHICS_DATA.bufferAttribs.emplace_back(MakeURef<VkBufferAttrib>(info, fullStride));
        return (BufferAttrib)GRAPHICS_DATA.bufferAttribs.size();
    }

    uint32_t GetVertBufferAttribSize(BufferAttrib attrib)
    {
        auto& attribute = GetAttrib(attrib);
        return attribute->vertexSize;
    }

    Buffer DefineUniformBuffer(const BufferDefinitionInfo& info)
    {
        GRAPHICS_DATA.boundBuffers.emplace_back(MakeURef<VkUniformBuffer>(info));
        return (Buffer)GRAPHICS_DATA.boundBuffers.size();
    }

    Buffer DefineStorageBuffer(const StorageBufferDefinitionInfo& info)
    {
        GRAPHICS_DATA.boundBuffers.emplace_back(MakeURef<VkStorageBuffer>(info));
        return (Buffer)GRAPHICS_DATA.boundBuffers.size();
    }

    void UploadBuffer(const void* rn, const BufferUploadInfo_i& info)
    {
        UploadBuffers((VkRenderer*)rn, info);
    }

    void BindBuffer(const void* rn, const BindResourceInfo_i<Buffer>& info)
    {
        hf::BindBuffer((VkRenderer*)rn, info);
    }

    ShaderLayout DefineShaderLayout(const ShaderLayoutDefinitionInfo& info)
    {
        GRAPHICS_DATA.shaderLayouts.emplace_back(MakeURef<VkShaderLayout>(info));
        return (ShaderLayout)GRAPHICS_DATA.shaderLayouts.size();
    }

    void BindShaderLayout(void* rn, ShaderLayout setup)
    {
        hf::BindShaderLayout((VkRenderer*)rn, setup);
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

    void UploadVertBuffer(const void* rn, const VertBufferUploadInfo_i& info)
    {
        hf::UploadBuffer((VkRenderer*)rn, (VkVertBuffer*)info.buffer, info.data, info.offset, info.vertexCount);
    }

    void* CreateIndexBuffer(const IndexBufferCreationInfo& info)
    {
        return new VkIndexBuffer(info);
    }

    void DestroyIndexBuffer(void* handle)
    {
        delete (VkIndexBuffer*)handle;
    }

    void UploadIndexBuffer(const void* rn, const IndexBufferUploadInfo_i& info)
    {
        hf::UploadBuffer((VkRenderer*)rn, (VkIndexBuffer*)info.buffer, info.data, info.offset, info.indexCount);
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

    void StartFrame(void* rn)
    {
        auto renderer = (VkRenderer*)rn;
        StartFrame(renderer);
    }

    void EndFrame(void* rn)
    {
        auto renderer = (VkRenderer*)rn;
        renderer->prevRenderTexture = nullptr;
        renderer->currentRenderTexture = nullptr;
        EndFrame(renderer);
    }

    void Draw(void* rn, const DrawCallInfo_i& info)
    {
        auto* vrn = (VkRenderer*)rn;

        uint32_t offset = 0;
        uint32_t vertCount = 0;
        for (uint32_t i = 0; i < info.bufferCount; i++)
        {
            auto vertBuffer = (VkVertBuffer*)info.pVertBuffers[i];
            vrn->vertBufferCache[i] = vertBuffer->buffers[vrn->currentFrame];
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
            drawInfo.indexBuffer = indexBuffer->buffers[vrn->currentFrame];
            drawInfo.indexType = indexBuffer->indexType;
            drawInfo.indexCount = indexBuffer->indexCount;
        }

        Draw(drawInfo);
    }

    void ApplyRenderAttachmentDependencies(void* rn, RenderAttachmentDependencyInfo* pInfos, uint32_t count)
    {
        const auto* vrn = (VkRenderer*)rn;
        GRAPHICS_DATA.preAllocBuffers.imageBarriers.reserve(count);
        GRAPHICS_DATA.preAllocBuffers.imageBarriers.clear();

        for (uint32_t i = 0; i < count; i++)
        {
            auto& info = pInfos[i];
            VkImageMemoryBarrier2 barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
            barrier.srcStageMask = (VkPipelineStageFlags2)info.src.stageMask;
            barrier.srcAccessMask = (VkAccessFlags2)info.src.accessMask;
            barrier.dstStageMask = (VkPipelineStageFlags2)info.dst.stageMask;
            barrier.dstAccessMask = (VkAccessFlags2)info.dst.accessMask;
            barrier.oldLayout = (VkImageLayout)info.src.targetLayout;
            barrier.newLayout = (VkImageLayout)info.dst.targetLayout;
            barrier.image = GetRenderTextureImage(vrn->prevRenderTexture, info.attachmentIndex);

            barrier.subresourceRange.aspectMask = (VkImageAspectFlags)info.aspectFlags;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.layerCount = 1;
            GRAPHICS_DATA.preAllocBuffers.imageBarriers.push_back(barrier);
        }

        VkDependencyInfo depInfo{};
        depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        depInfo.imageMemoryBarrierCount = GRAPHICS_DATA.preAllocBuffers.imageBarriers.size();
        depInfo.pImageMemoryBarriers = GRAPHICS_DATA.preAllocBuffers.imageBarriers.data();

        GRAPHICS_DATA.extensionFunctions.vkCmdPipelineBarrier2KHR(vrn->currentCommand, &depInfo);
    }

    void WaitForDevice()
    {
        hf::WaitForDevice();
    }

    void BeginRendering(void* rn, void* tex)
    {
        const auto vrn = (VkRenderer*)rn;

        if (vrn->currentRenderTexture)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot begin rendering while rendering is in progress");

        vrn->currentRenderTexture = (VkRenderTexture*)tex;
        hf::BeginRendering(vrn);
    }

    void EndRendering(void* rn)
    {
        const auto vrn = (VkRenderer*)rn;

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

            .CreateShaderLibrary        = CreateShaderLibrary,
            .DestroyShaderLibrary       = DestroyShaderLibrary,

            //shader
            .CreateShader               = CreateShader,
            .DestroyShader              = DestroyShader,
            .BindShader                 = BindShader,

            //uniform storage
            .DefineShaderLayout         = DefineShaderLayout,
            .BindShaderLayout           = BindShaderLayout,
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
