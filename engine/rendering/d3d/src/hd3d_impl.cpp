#include "hd3d_graphics.h"
#include "hd3d_renderer.h"

namespace hf::inter::rendering
{
    void Load(const RendererLoadInfo& info)
    {
        Direct3DLoad();
    }

    void Unload()
    {
        Direct3DUnload();
        GRAPHICS_DATA = {};
    }

    void* CreateInstance(const RendererInstanceCreationInfo& info)
    {
        return hf::CreateRenderer((HWND)info.handle, info.size, info.vSyncMode);
    }

    void PostInstanceLoad(void* rn, RenderPass pass)
    {

    }

    void DestroyInstance(void* rnInstance)
    {
        hf::DestroyRenderer((D3DRenderer*)rnInstance);
    }

    RenderPass DefineRenderPass(const RenderPassDefinitionInfo& info)
    {
        return 1;
    }

    void BindRenderPass(void* rn, RenderPass pass)
    {

    }

    void BeginRenderPass(void* rn, RenderPass pass)
    {

    }

    void EndRenderPass(void* rn)
    {

    }

    void* CreateShader(const ShaderCreationInfo& info)
    {
        return nullptr;
    }

    void DestroyShader(void* shader)
    {
    }

    void BindShader(const void* renderer, const ShaderBindingInfo& info)
    {

    }

    void* CreateTexture(const TextureCreationInfo& info)
    {
        return nullptr;
    }

    void DestroyTexture(void* tex)
    {

    }

    void* CreateTexturePack(const TexturePackCreationInfo& info)
    {
        return nullptr;
    }

    void DestroyTexturePack(void* txPack)
    {

    }

    void UploadTexturePackBinding(void* texPack, const TexturePackBindingUploadGroupInfo& info)
    {

    }

    void BindTexturePack(void* rn, const TexturePackBindingInfo& info)
    {

    }

    void* CreateTexturePackAllocator(const TexturePackAllocatorCreationInfo& info)
    {
        return nullptr;
    }

    void DestroyTexturePackAllocator(void* texPackAllocator)
    {

    }

    uint32_t DefineTextureSampler(const TextureSamplerDefinitionInfo& info)
    {
        return 1;
    }

    uint32_t DefineTextureLayout(const TextureLayoutDefinitionInfo& info)
    {
        return 1;
    }

    uint32_t DefineVertBufferAttrib(const BufferAttribDefinitionInfo& info, uint32_t fullStride)
    {
        return 1;
    }

    uint32_t GetVertBufferAttribSize(BufferAttrib attrib)
    {
        return 0;
    }

    uint32_t DefineUniformBuffer(const UniformBufferDefinitionInfo& info)
    {
        return 1;
    }

    void UploadUniformBuffer(const void* rn, const UniformBufferUploadInfo& info)
    {

    }

    void BindUniformBuffer(const void* rn, const UniformBufferBindInfo& info)
    {

    }

    uint32_t DefineShaderSetup(const ShaderSetupDefinitionInfo& info)
    {
        return 1;
    }

    uint32_t DefineUniformAllocator(const UniformAllocatorDefinitionInfo& info)
    {
        return 1;
    }

    void BindShaderSetup(void* rn, ShaderSetup)
    {

    }

    void UploadPushConstants(void* rn, const PushConstantUploadInfo& info)
    {

    }

    void* CreateVertBuffer(const VertBufferCreationInfo& info)
    {
        return nullptr;
    }

    void DestroyVertBuffer(void* handle)
    {

    }

    void UploadVertBuffer(const VertBufferUploadInfo& info)
    {

    }

    void* CreateIndexBuffer(const IndexBufferCreationInfo& info)
    {
        return nullptr;
    }

    void DestroyIndexBuffer(void* handle)
    {

    }

    void UploadIndexBuffer(const IndexBufferUploadInfo& info)
    {

    }

    void* CreateStorageBuffer(const StorageBufferCreationInfo& info)
    {
        return nullptr;
    }

    void DestroyStorageBuffer(void* handle)
    {

    }

    void UploadStorageBuffer(const StorageBufferUploadInfo& info)
    {

    }

    void SubmitBufferCopyOperations()
    {

    }

    void SubmitTextureCopyOperations()
    {

    }

    void WaitForPreviousFrame(void* rn)
    {

    }

    bool GetReadyForRendering(void* rn)
    {
        auto renderer = (D3DRenderer*)rn;
        return GetReadyForRendering(renderer);
    }

    void StartFrame(void* rn)
    {
        auto renderer = (D3DRenderer*)rn;
        StartFrame(renderer);
    }

    void EndFrame(void* rn)
    {
        auto renderer = (D3DRenderer*)rn;
        EndFrame(renderer);
    }

    void RegisterFrameBufferChange(void* rn, uvec2 newSize)
    {
        auto renderer = (D3DRenderer*)rn;
        RegisterFrameBufferChange(renderer, newSize);
    }

    void SetVSync(void* rn, VsyncMode mode)
    {

    }

    void Draw(void* rn, const DrawCallInfo& info)
    {
        auto renderer = (D3DRenderer*)rn;
        Draw(renderer);
    }

    void WaitForDevice()
    {

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
            .UploadPushConstants        = UploadPushConstants,

            //texture
            .CreateTexture              = CreateTexture,
            .DestroyTexture             = DestroyTexture,

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

            //uniform buffer
            .DefineUniformBuffer        = DefineUniformBuffer,
            .UploadUniformBuffer        = UploadUniformBuffer,
            .BindUniformBuffer          = BindUniformBuffer,

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
            .WaitForPreviousFrame       = WaitForPreviousFrame,
            .GetReadyForRendering       = GetReadyForRendering,
            .StartFrame                 = StartFrame,
            .EndFrame                   = EndFrame,
            .Draw                       = Draw,
            .WaitForDevice              = WaitForDevice,

            .RegisterFrameBufferChange  = RegisterFrameBufferChange,
            .SetVSync                   = SetVSync,
        };
        return &api;
    }
}