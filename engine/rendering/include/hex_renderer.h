#ifndef RENDEREREXPORT_H
#define RENDEREREXPORT_H

#include "hshared.h"
#include "../../others/exception/include/hgenericexception.h"

namespace hf::inter::rendering
{
#define NUM_DEPTH_STENCIL_FORMATS 3
    constexpr TextureFormat DEPTH_STENCIL_FORMATS[]
    {
        TextureFormat::D32_Sfloat_S8_Uint,
        TextureFormat::D24_Unorm_S8_Uint,
        TextureFormat::D16_Unorm_S8_Uint,
    };

#define NUM_DEPTH_FORMATS 5
    constexpr TextureFormat DEPTH_FORMATS[]
    {
        TextureFormat::D32_Sfloat,
        TextureFormat::D32_Sfloat_S8_Uint,
        TextureFormat::D24_Unorm_S8_Uint,
        TextureFormat::D16_Unorm_S8_Uint,
        TextureFormat::D16_Unorm,
    };

#define NUM_STENCIL_FORMATS 4
    constexpr TextureFormat STENCIL_FORMATS[]
    {
        TextureFormat::S8_Uint,
        TextureFormat::D16_Unorm_S8_Uint,
        TextureFormat::D24_Unorm_S8_Uint,
        TextureFormat::D32_Sfloat_S8_Uint,
    };

    struct ShaderCreationInfo
    {
        RenderPass drawPass;
        ShaderSetup shaderSetup{};
        void* texPack{};
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
        RenderPass (*onPassCreationCallback)();
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
        void* handle{};
        uvec2 size{};
        bool vSyncOn = false;
    };

    struct TextureCreationInfo
    {
        uvec3 size;
        TextureChannel channel;
        uint32_t mipLevels = 1;
        void* data{};
        TextureType type = TextureType::Tex2D;
        TextureFormat format = TextureFormat::B8G8R8A8_Srgb;
        BufferMemoryType memoryType = BufferMemoryType::Static;
    };

    struct TextureAllocatorCreationInfo
    {
        void** pTextures{};
        uint32_t textureCount = 0;
    };

    struct TexturePackBinding
    {
        uint32_t bindingId = 0;
        TextureSampler sampler{};
        std::vector<void*> textures{};
    };

    struct TexturePackCreationInfo
    {
        RenderBindingType bindingType = RenderBindingType::Graphics;
        uint32_t setBindingIndex = 0;
        TexturePackBinding* pBindings{};
        uint32_t bindingCount = 0;
        TextureLayout layout = 0;
    };

    struct TexturePackUploadInfo
    {
        uint32_t bindingIndex = 0;
        void** pTextures{};
        uint32_t textureOffset = 0;
        uint32_t textureCount = 0;
        TextureSampler sampler{};
    };

    struct TexturePackAllocatorCreationInfo
    {
        void** pTexturePacks{};
        uint32_t texturePackCount = 0;
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

        RenderPass (*DefineRenderPass)(const RenderPassDefinitionInfo& info);
        void (*BeginRenderPass)(void* rn, RenderPass pass);
        void (*EndRenderPass)(void* rn);

        //Shaders
        void* (*CreateShader)(const ShaderCreationInfo& info);
        void (*DestroyShader)(void* shader);
        void (*BindShader)(const void* rn, const void* shader, BufferAttrib attrib);

        //shader setup
        ShaderSetup (*DefineShaderSetup)(const ShaderSetupDefinitionInfo& info);
        void (*BindShaderSetup)(void* rn, ShaderSetup);

        //texture
        void* (*CreateTexture)(const TextureCreationInfo& info);
        void (*DestroyTexture)(void* tex);

        //texture allocator
        void* (*CreateTextureAllocator)(const TextureAllocatorCreationInfo& info);
        void (*DestroyTextureAllocator)(void* texAlloc);

        //texture pack
        void* (*CreateTexturePack)(const TexturePackCreationInfo& info);
        void (*DestroyTexturePack)(void* texPack);
        void (*UploadTexturePack)(void* texPack, const TexturePackUploadInfo& info);
        void (*BindTexturePack)(void* rn, void* texPack);

        //texture pack allocator
        void* (*CreateTexturePackAllocator)(const TexturePackAllocatorCreationInfo& info);
        void (*DestroyTexturePackAllocator)(void* texPackAllocator);

        //texture sampler
        TextureSampler (*DefineTextureSampler)(const TextureSamplerDefinitionInfo& info);

        //texture layout
        TextureLayout (*DefineTextureLayout)(const TextureLayoutDefinitionInfo& info);

        //buffer attribute
        BufferAttrib (*DefineVertBufferAttrib)(const BufferAttribDefinitionInfo& info, uint32_t fullStride);
        uint32_t (*GetVertBufferAttribSize)(BufferAttrib attrib);

        //uniform buffer
        UniformBuffer (*DefineUniformBuffer)(const UniformBufferDefinitionInfo& info);
        void (*UploadUniformBuffer)(const void* rn, const UniformBufferUploadInfo& info);

        //uniform allocator
        UniformAllocator (*DefineUniformAllocator)(const UniformAllocatorDefinitionInfo& info);

        //vertex buffer
        void* (*CreateVertBuffer)(const VertBufferCreationInfo& info);
        void (*DestroyVertBuffer)(void* handle);
        void (*UploadVertBuffer)(const VertBufferUploadInfo& info);

        //index buffer
        void* (*CreateIndexBuffer)(const IndexBufferCreationInfo& info);
        void (*DestroyIndexBuffer)(void* handle);
        void (*UploadIndexBuffer)(const IndexBufferUploadInfo& info);

        //copy operations
        void (*SubmitBufferCopyOperations)();
        void (*SubmitTextureCopyOperations)();

        //RenderingOperations
        bool (*GetReadyForRendering)(void* rn);
        void (*StartFrame)(void* rn);
        void (*EndFrame)(void* rn);
        void (*Draw)(void* rn, const DrawCallInfo& info);
        void (*WaitForRendering)();

        void (*RegisterFrameBufferChange)(void* rn, uvec2 newSize);
        void (*SetVSync)(void* rn, bool isOn);
    };
}

#endif //RENDEREREXPORT_H
