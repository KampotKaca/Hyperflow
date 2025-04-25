#ifndef RENDEREREXPORT_H
#define RENDEREREXPORT_H

#include "../hshared.h"
#include "hgenericexception.h"

namespace hf::inter::rendering
{
#ifdef RENDERER_DLL
#define RENDERER_API __declspec(dllexport)
#else
#define RENDERER_API __declspec(dllimport)
#endif

    struct ShaderCreationInfo
    {
        uint32_t supportedAttribCount{};
        const BufferAttrib* pSupportedAttribs{};

        const char* vCode{};
        uint32_t vCodeSize = 0;
        const char* fCode{};
        uint32_t fCodeSize = 0;
    };

    struct RendererLoadInfo
    {
        uvec3 appVersion;
        uvec3 engineVersion;
        const char* applicationTitle;
    };

    RENDERER_API void Load(const RendererLoadInfo& info);
    RENDERER_API void Unload();
    RENDERER_API void* CreateInstance(void* handle, uvec2 size);
    RENDERER_API void DestroyInstance(void* rnInstance);
    RENDERER_API void* CreateShader(const ShaderCreationInfo& info);
    RENDERER_API void DestroyShader(void* shader);

    RENDERER_API void BindShader(const void* renderer, const void* shader, BufferAttrib attrib);
    RENDERER_API uint32_t CreateBufferAttrib(const BufferAttribCreateInfo& info, uint32_t fullStride);
    RENDERER_API void* CreateVertBuffer(const VertBufferCreationInfo& info);

    RENDERER_API void DestroyVertBuffer(void* handle);
    RENDERER_API bool StartFrame(void* rn);
    RENDERER_API void EndFrame(void* rn);

    RENDERER_API void RegisterFrameBufferChange(void* rn, uvec2 newSize);
    RENDERER_API void Draw(void* rn);
    RENDERER_API void WaitForRendering();
}

#endif //RENDEREREXPORT_H
