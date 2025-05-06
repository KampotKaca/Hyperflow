#ifndef RENDEREREXPORT_H
#define RENDEREREXPORT_H

#include "hshared.h"
#include "../../others/exception/include/hgenericexception.h"

namespace hf::inter::rendering
{
    struct ShaderCreationInfo
    {
        UniformStorage uniformStorage{};
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
        void* platformDll;
        void* (*getFuncFromDll)(void* dll, const char* funcName);
    };

    struct VertBufferUploadInfo
    {
        const void* buffer;
        const void* data;
        uint32_t offset;
        uint32_t vertexCount;
    };

    struct IndexBufferUploadInfo
    {
        const void* buffer;
        const void* data;
        uint32_t offset;
        uint32_t indexCount;
    };

    struct RendererInstanceCreationInfo
    {
        void* handle;
        uvec2 size;
    };

    struct TextureCreationInfo
    {
        uvec3 size;
        TextureChannel channel;
        uint32_t mipLevels = 1;
        void* data{};
        TextureFormat format = TextureFormat::B8G8R8A8_Srgb;
        TextureSampler sampler{};
    };

    struct TexturePackCreationInfo
    {
        TextureCreationInfo* pTextures{};
        uint32_t textureCount = 0;
        TextureType type = TextureType::Tex2D;
        BufferMemoryType memoryType = BufferMemoryType::Static;
    };

    struct DrawCallInfo
    {
        void** pVertBuffers;
        uint32_t bufferCount;

        void* indexBuffer;
        uint32_t instanceCount;
    };

    struct RendererAPI
    {
        //Creation
        void (*Load)(const RendererLoadInfo& info);
        void (*Unload)();
        void* (*CreateInstance)(const RendererInstanceCreationInfo& info);
        void (*DestroyInstance)(void* rn);

        //Shaders
        void* (*CreateShader)(const ShaderCreationInfo& info);
        void (*DestroyShader)(void* shader);
        void (*BindShader)(const void* rn, const void* shader, BufferAttrib attrib);

        //texture pack
        void* (*CreateTexturePack)(const TexturePackCreationInfo& info);
        void (*DestroyTexturePack)(void* txPack);

        //texture sampler
        uint32_t (*DefineTextureSampler)(const TextureSamplerDefinitionInfo& info);

        //buffer attribute
        uint32_t (*DefineVertBufferAttrib)(const BufferAttribDefinitionInfo& info, uint32_t fullStride);
        uint32_t (*GetVertBufferAttribSize)(BufferAttrib attrib);

        //uniform buffer
        uint32_t (*DefineUniformBuffer)(const UniformBufferDefinitionInfo& info);
        void (*UploadUniformBuffer)(const void* rn, const UniformBufferUploadInfo& info);

        //uniform storage
        uint32_t (*DefineUniformStorage)(const UniformStorageDefinitionInfo& info);
        void (*BindUniformStorage)(void* rn, UniformStorage);

        //uniform allocator
        uint32_t (*DefineUniformAllocator)(const UniformAllocatorDefinitionInfo& info);

        //vertex buffer
        void* (*CreateVertBuffer)(const VertBufferCreationInfo& info);
        void (*DestroyVertBuffer)(void* handle);
        void (*UploadVertBuffer)(const VertBufferUploadInfo& info);

        //index buffer
        void* (*CreateIndexBuffer)(const IndexBufferCreationInfo& info);
        void (*DestroyIndexBuffer)(void* handle);
        void (*UploadIndexBuffer)(const IndexBufferUploadInfo& info);
        void (*SubmitStagedCopyOperations)();

        //RenderingOperations
        bool (*GetReadyForRendering)(void* rn);
        void (*StartFrame)(void* rn);
        void (*EndFrame)(void* rn);
        void (*RegisterFrameBufferChange)(void* rn, uvec2 newSize);
        void (*Draw)(void* rn, const DrawCallInfo& info);
        void (*WaitForRendering)();
    };
}

#endif //RENDEREREXPORT_H
