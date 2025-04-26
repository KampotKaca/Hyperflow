#include "hvk_graphics.h"
#include "hvk_renderer.h"
#include "hvk_shader.h"
#include "hvk_vertbuffer.h"

namespace hf::inter::rendering
{
    void Load(const RendererLoadInfo& info)
    {
        LoadVulkan(info);
    }

    void Unload()
    {
        UnloadVulkan();
    }

    void* CreateInstance(const RendererInstanceCreationInfo& info)
    {
        return new VKRenderer(info.handle, info.size);
    }

    void DestroyInstance(void* rnInstance)
    {
        auto data = (VKRenderer*)rnInstance;
        DelayThreadUntilRenderingFinish();
        delete(data);
    }

    void* CreateShader(const ShaderCreationInfo& info)
    {
        return new VkShader(info);
    }

    void DestroyShader(void* shader)
    {
        delete (VkShader*)shader;
    }

    void BindShader(const void* renderer, const void* shader, BufferAttrib attrib)
    {
        BindShader((VKRenderer*)renderer, (VkShader*)shader, attrib);
    }

    uint32_t CreateBufferAttrib(const BufferAttribCreateInfo& info, uint32_t fullStride)
    {
        GRAPHICS_DATA.bufferAttribs.emplace_back(info, fullStride);
        return GRAPHICS_DATA.bufferAttribs.size();
    }

    void* CreateVertBuffer(const VertBufferCreationInfo& info)
    {
        return new VkVertBuffer(info);
    }

    void DestroyVertBuffer(void* handle)
    {
        delete (VkVertBuffer*)handle;
    }

    bool StartFrame(void* rn)
    {
        auto renderer = (VKRenderer*)rn;
        return StartFrame(renderer);
    }

    void EndFrame(void* rn)
    {
        auto renderer = (VKRenderer*)rn;
        EndFrame(renderer);
    }

    void RegisterFrameBufferChange(void* rn, uvec2 newSize)
    {
        auto renderer = (VKRenderer*)rn;
        RegisterFrameBufferChange(renderer, newSize);
    }

    void Draw(void* rn)
    {
        auto renderer = (VKRenderer*)rn;
        Draw(renderer);
    }

    void WaitForRendering()
    {
        DelayThreadUntilRenderingFinish();
    }

    extern "C" __declspec(dllexport)
    RendererAPI* GetAPI()
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