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

    uint32_t CreateBufferAttrib(const BufferAttribDefinitionInfo& info, uint32_t fullStride)
    {
        return 1;
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

    void Draw(const DrawCallInfo& info)
    {
        auto renderer = (D3DRenderer*)info.renderer;
        Draw(renderer);
    }

    void WaitForRendering()
    {

    }

    API RendererAPI* GetAPI()
    {
        static RendererAPI api =
        {
            .Load = &Load,
            .Unload = &Unload,
            .CreateInstance = &CreateInstance,
            .DestroyInstance = &DestroyInstance,

            .CreateShader = &CreateShader,
            .DestroyShader = &DestroyShader,
            .BindShader = &BindShader,

            .DefineVertBufferAttrib = &CreateBufferAttrib,
            .CreateVertBuffer = &CreateVertBuffer,
            .DestroyVertBuffer = &DestroyVertBuffer,
            .UploadVertBuffer = &UploadVertBuffer,
            .CreateIndexBuffer = &CreateIndexBuffer,
            .DestroyIndexBuffer = &DestroyIndexBuffer,
            .UploadIndexBuffer = &UploadIndexBuffer,

            .SubmitStagedCopyOperations = &SubmitStagedCopyOperations,
            .GetReadyForRendering = &GetReadyForRendering,
            .StartFrame = &StartFrame,
            .EndFrame = &EndFrame,
            .RegisterFrameBufferChange = &RegisterFrameBufferChange,
            .Draw = &Draw,
            .WaitForRendering = &WaitForRendering
        };
        return &api;
    }
}