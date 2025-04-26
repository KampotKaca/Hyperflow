#include "hd3d_graphics.h"
#include "hd3d_renderer.h"

namespace hf::inter::rendering
{
    extern "C"
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
            return CreateRenderer((HWND)info.handle);
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

        bool StartFrame(void* rn)
        {
            auto renderer = (D3DRenderer*)rn;
            return StartFrame(renderer);
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

        void Draw(void* rn)
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
                &StartFrame,
                &EndFrame,
                &RegisterFrameBufferChange,
                &Draw,
                &WaitForRendering
            };
            return &api;
        }
    }
}