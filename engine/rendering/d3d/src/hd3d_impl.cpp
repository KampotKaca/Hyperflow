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

    uint32_t CreateBufferAttrib(const BufferAttribCreateInfo& info, uint32_t fullStride)
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