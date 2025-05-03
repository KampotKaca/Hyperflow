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
        return hf::CreateRenderer((HWND)info.handle);
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

    void BindShader(const void* renderer, const void* shader, BufferAttrib attrib)
    {

    }

    void* CreateTexturePack(const TexturePackCreationInfo& info)
    {
        return nullptr;
    }

    void DestroyTexturePack(void* txPack)
    {

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

    uint32_t DefineUniformStorage(const UniformStorageDefinitionInfo& info)
    {
        return 1;
    }

    uint32_t DefineUniformAllocator(const UniformAllocatorDefinitionInfo& info)
    {
        return 1;
    }

    void BindUniformStorage(void* rn, UniformStorage)
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

    void SubmitStagedCopyOperations()
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

    void Draw(void* rn, const DrawCallInfo& info)
    {
        auto renderer = (D3DRenderer*)rn;
        Draw(renderer);
    }

    void WaitForRendering()
    {

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

            //texture pack
            .CreateTexturePack          = &CreateTexturePack,
            .DestroyTexturePack         = &DestroyTexturePack,

            //buffer attribute
            .DefineVertBufferAttrib     = &DefineVertBufferAttrib,
            .GetVertBufferAttribSize    = &GetVertBufferAttribSize,

            //uniform buffer
            .DefineUniformBuffer        = &DefineUniformBuffer,
            .UploadUniformBuffer        = &UploadUniformBuffer,

            //uniform storage
            .DefineUniformStorage       = &DefineUniformStorage,
            .BindUniformStorage         = &BindUniformStorage,

            //uniform allocator
            .DefineUniformAllocator     = &DefineUniformAllocator,

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