#include "include/hd3d_graphics.h"
#include "include/hd3d_renderer.h"

namespace hf::inter::rendering
{
    void Load(const char* version)
    {
        Direct3DLoad();
    }

    void Unload()
    {
        Direct3DUnload();
    }

    void* CreateInstance(void* handle, uvec2 size)
    {
        return CreateRenderer((HWND)handle);
    }

    void DestroyInstance(void* rnInstance)
    {
        DestroyRenderer((D3DRenderer*)rnInstance);
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

    bool StartFrame(Renderer* rn)
    {
        auto renderer = (D3DRenderer*)rn->handle;
        return StartFrame(renderer);
    }

    void EndFrame(Renderer* rn)
    {
        auto renderer = (D3DRenderer*)rn->handle;
        EndFrame(renderer);
    }

    void RegisterFrameBufferChange(Renderer* rn, uvec2 newSize)
    {
        auto renderer = (D3DRenderer*)rn->handle;
        RegisterFrameBufferChange(renderer, newSize);
    }

    void Draw(Renderer* rn)
    {
        auto renderer = (D3DRenderer*)rn->handle;
        Draw(renderer);
    }

    void WaitForRendering()
    {

    }
}