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

    void DestroyInstance(void* rnInstance)
    {
        hf::DestroyRenderer((D3DRenderer*)rnInstance);
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

    Buffer DefineUniformBuffer(const BufferDefinitionInfo& info)
    {
        return 1;
    }

    Buffer DefineStorageBuffer(const StorageBufferDefinitionInfo& info)
    {
        return 1;
    }

    void UploadBuffer(const void* rn, const BufferUploadInfo& info)
    {

    }

    void BindBuffer(const void* rn, const BufferBindInfo& info)
    {

    }

    BufferAllocator DefineBufferAllocator(const BufferAllocatorDefinitionInfo& info)
    {
        return 1;
    }

    uint32_t DefineShaderLayout(const ShaderLayoutDefinitionInfo& info)
    {
        return 1;
    }

    void BindShaderLayout(void* rn, ShaderLayout layout)
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

    void SubmitBufferCopyOperations()
    {

    }

    void SubmitTextureCopyOperations()
    {

    }

    void WaitForPreviousFrame(void* rn)
    {

    }

    uvec2 GetReadyForRendering(void* rn, void** pTextures, uint32_t textureCount)
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

    void* GetCmd(void* rn)
    {
        return nullptr;
    }

    void Draw(void* rn, const DrawCallInfo& info)
    {
        auto renderer = (D3DRenderer*)rn;
        Draw(renderer);
    }

    void WaitForDevice()
    {

    }

    void* GetEditorInfo()
    {
        return nullptr;
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
            .DefineShaderLayout         = DefineShaderLayout,
            .BindShaderLayout           = BindShaderLayout,
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
            .WaitForDevice              = WaitForDevice,

            .RegisterFrameBufferChange  = RegisterFrameBufferChange,
            .SetVSync                   = SetVSync,
            .GetCmd                     = GetCmd,

            .GetEditorInfo              = GetEditorInfo,
        };
        return &api;
    }
}