#ifndef RENDEREREXPORT_H
#define RENDEREREXPORT_H

#include "../hshared.h"
#include "../../others/exception/include/hgenericexception.h"

namespace hf::inter::rendering
{
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
        void* platformInstance;
    };

    struct RendererInstanceCreationInfo
    {
        void* handle;
        uvec2 size;
    };

    struct DrawCallInfo
    {
        void* renderer;
        void** pBuffers;
        uint32_t bufferCount;
        uint32_t instanceCount;
    };

    struct RendererAPI
    {
        void (*Load)(const RendererLoadInfo& info);
        void (*Unload)();
        void* (*CreateInstance)(const RendererInstanceCreationInfo& info);
        void (*DestroyInstance)(void* rnInstance);
        void* (*CreateShader)(const ShaderCreationInfo& info);
        void (*DestroyShader)(void* shader);
        void (*BindShader)(const void* renderer, const void* shader, BufferAttrib attrib);
        uint32_t (*CreateBufferAttrib)(const BufferAttribCreateInfo& info, uint32_t fullStride);
        void* (*CreateVertBuffer)(const VertBufferCreationInfo& info);
        void (*DestroyVertBuffer)(void* handle);
        bool (*StartFrame)(void* rn);
        void (*EndFrame)(void* rn);
        void (*RegisterFrameBufferChange)(void* rn, uvec2 newSize);
        void (*Draw)(const DrawCallInfo& info);
        void (*WaitForRendering)();
    };
}

#endif //RENDEREREXPORT_H
