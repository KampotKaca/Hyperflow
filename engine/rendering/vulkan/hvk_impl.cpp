#include "include/hvk_graphics.h"
#include "include/hvk_renderer.h"
#include "include/hvk_shader.h"
#include "include/hvk_vertbuffer.h"

namespace hf::inter::rendering
{
    RENDERER_API void Load(const RendererLoadInfo& info)
    {
        LoadVulkan(info);
    }

    RENDERER_API void Unload()
    {
        UnloadVulkan();
    }

    RENDERER_API void* CreateInstance(void* handle, uvec2 size)
    {
        return new VKRenderer(handle, size);
    }

    RENDERER_API void DestroyInstance(void* rnInstance)
    {
        auto data = (VKRenderer*)rnInstance;
        WaitForRendering();
        delete(data);
    }

    RENDERER_API void* CreateShader(const ShaderCreationInfo& info)
    {
        return new VkShader(info);
    }

    RENDERER_API void DestroyShader(void* shader)
    {
        delete (VkShader*)shader;
    }

    RENDERER_API void BindShader(const void* renderer, const void* shader, BufferAttrib attrib)
    {
        BindShader((VKRenderer*)renderer, (VkShader*)shader, attrib);
    }

    RENDERER_API uint32_t CreateBufferAttrib(const BufferAttribCreateInfo& info, uint32_t fullStride)
    {
        GRAPHICS_DATA.bufferAttribs.emplace_back(info, fullStride);
        return GRAPHICS_DATA.bufferAttribs.size();
    }

    RENDERER_API void* CreateVertBuffer(const VertBufferCreationInfo& info)
    {
        return new VkVertBuffer(info);
    }

    RENDERER_API void DestroyVertBuffer(void* handle)
    {
        delete (VkVertBuffer*)handle;
    }

    RENDERER_API bool StartFrame(void* rn)
    {
        auto renderer = (VKRenderer*)rn;
        return StartFrame(renderer);
    }

    RENDERER_API void EndFrame(void* rn)
    {
        auto renderer = (VKRenderer*)rn;
        EndFrame(renderer);
    }

    RENDERER_API void RegisterFrameBufferChange(void* rn, uvec2 newSize)
    {
        auto renderer = (VKRenderer*)rn;
        RegisterFrameBufferChange(renderer, newSize);
    }

    RENDERER_API void Draw(void* rn)
    {
        auto renderer = (VKRenderer*)rn;
        Draw(renderer);
    }

    RENDERER_API void WaitForRendering()
    {
        DelayThreadUntilRenderingFinish();
    }
}