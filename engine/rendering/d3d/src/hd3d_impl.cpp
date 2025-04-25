#include "../include/hd3d_graphics.h"
#include "../include/hd3d_renderer.h"

namespace hf::inter::rendering
{
    extern "C"
    {
        RENDERER_API void Load(const RendererLoadInfo& info)
        {
            Direct3DLoad();
        }

        RENDERER_API void Unload()
        {
            Direct3DUnload();
        }

        RENDERER_API void* CreateInstance(void* handle, uvec2 size)
        {
            return CreateRenderer((HWND)handle);
        }

        RENDERER_API void DestroyInstance(void* rnInstance)
        {
            DestroyRenderer((D3DRenderer*)rnInstance);
        }

        RENDERER_API void* CreateShader(const ShaderCreationInfo& info)
        {
            return nullptr;
        }

        RENDERER_API void DestroyShader(void* shader)
        {
        }

        RENDERER_API void BindShader(const void* renderer, const void* shader, BufferAttrib attrib)
        {

        }

        RENDERER_API uint32_t CreateBufferAttrib(const BufferAttribCreateInfo& info, uint32_t fullStride)
        {
            return 1;
        }

        RENDERER_API void* CreateVertBuffer(const VertBufferCreationInfo& info)
        {
            return nullptr;
        }

        RENDERER_API void DestroyVertBuffer(void* handle)
        {

        }

        RENDERER_API bool StartFrame(void* rn)
        {
            auto renderer = (D3DRenderer*)rn;
            return StartFrame(renderer);
        }

        RENDERER_API void EndFrame(void* rn)
        {
            auto renderer = (D3DRenderer*)rn;
            EndFrame(renderer);
        }

        RENDERER_API void RegisterFrameBufferChange(void* rn, uvec2 newSize)
        {
            auto renderer = (D3DRenderer*)rn;
            RegisterFrameBufferChange(renderer, newSize);
        }

        RENDERER_API void Draw(void* rn)
        {
            auto renderer = (D3DRenderer*)rn;
            Draw(renderer);
        }

        RENDERER_API void WaitForRendering()
        {

        }
    }
}