#include "include/hvk_graphics.h"
#include "include/hvk_renderer.h"
#include "include/hvk_shader.h"
#include "include/hvk_vertbuffer.h"

namespace hf::inter::rendering
{
    void Load(const char* version)
    {
        LoadVulkan(version);
    }

    void Unload()
    {
        UnloadVulkan();
    }

    void* CreateInstance(void* handle, uvec2 size)
    {
        return new VKRenderer(handle, size);
    }

    void DestroyInstance(void* rnInstance)
    {
        auto data = (VKRenderer*)rnInstance;
        WaitForRendering();
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

    bool StartFrame(Renderer* rn)
    {
        auto renderer = (VKRenderer*)rn->handle;
        return StartFrame(renderer);
    }

    void EndFrame(Renderer* rn)
    {
        auto renderer = (VKRenderer*)rn->handle;
        EndFrame(renderer);
    }

    void RegisterFrameBufferChange(Renderer* rn, uvec2 newSize)
    {
        auto renderer = (VKRenderer*)rn->handle;
        RegisterFrameBufferChange(renderer, newSize);
    }

    void Draw(Renderer* rn)
    {
        auto renderer = (VKRenderer*)rn->handle;
        Draw(renderer);
    }

    void WaitForRendering()
    {
        DelayThreadUntilRenderingFinish();
    }
}