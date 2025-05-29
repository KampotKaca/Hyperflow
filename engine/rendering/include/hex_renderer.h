#ifndef HEX_RENDERER_H
#define HEX_RENDERER_H

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
        RenderPass renderPass{};
        ShaderSetup shaderSetup{};
        void* texPack{};
        uint32_t supportedAttribCount{};
        const BufferAttrib* pSupportedAttribs{};

        const char* vCode{};
        uint32_t vCodeSize = 0;
        const char* fCode{};
        uint32_t fCodeSize = 0;

        ShaderDepthStencilOptions depthStencilOptions{};
        ShaderBlendingOptions blendingOptions{};
    };

    struct ShaderBindingInfo
    {
        void* shader{};
        BufferAttrib attrib{};
        RenderBindingType bindingPoint{};
    };

    struct RendererLoadInfo
    {
        uvec3 appVersion{};
        uvec3 engineVersion{};
        const char* applicationTitle{};
        uint32_t (*createVulkanSurfaceFunc)(void* windowHandle, void* instance, void* surfaceResult);
        void* platformDll{};
        void* (*getFuncFromDll)(void* dll, const char* funcName){};
    };

    struct VertBufferUploadInfo
    {
        const void* buffer{};
        const void* data{};
        uint32_t offset{};
        uint32_t vertexCount{};
    };

    struct IndexBufferUploadInfo
    {
        const void* buffer{};
        const void* data{};
        uint32_t offset{};
        uint32_t indexCount{};
    };

    struct StorageBufferUploadInfo
    {
        const void* storage{};
        const void* data{};
        uint64_t offsetInBytes{};
        uint64_t sizeInBytes{};
    };

    struct RendererInstanceCreationInfo
    {
        RenderPass mainPass{};
        void* handle{};
        uvec2 size{};
        VsyncMode vSyncMode = VsyncMode::NoSync;
    };

    struct TextureCreationInfo
    {
        uvec3 size = { 1, 1, 1 };
        TextureChannel channel = TextureChannel::RGBA;
        uint32_t mipLevels = 1;
        uint32_t samples = 1;
        void* data{};
        TextureDetails details{};
    };

    struct TexturePackBinding
    {
        TextureSampler sampler{};
        std::vector<void*> textures{};
    };

    struct TexturePackCreationInfo
    {
        RenderBindingType bindingType = RenderBindingType::Graphics;
        uint32_t bindingId = 0;
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
        void** pVertBuffers{};
        uint32_t bufferCount{};

        void* indexBuffer{};
        uint32_t instanceCount{};
    };

    struct RendererAPI
    {
        //Creation
        void (*Load)(const RendererLoadInfo& info);
        void (*Unload)();
        void* (*CreateInstance)(const RendererInstanceCreationInfo& info);
        void (*PostInstanceLoad)(void* rn, RenderPass pass);
        void (*DestroyInstance)(void* rn);

        RenderPass (*DefineRenderPass)(const RenderPassDefinitionInfo& info);
        void (*BindRenderPass)(void* rn, RenderPass pass);
        void (*BeginRenderPass)(void* rn, RenderPass pass);
        void (*EndRenderPass)(void* rn);

        //Shaders
        void* (*CreateShader)(const ShaderCreationInfo& info);
        void (*DestroyShader)(void* shader);
        void (*BindShader)(const void* rn, const ShaderBindingInfo& info);

        //shader setup
        ShaderSetup (*DefineShaderSetup)(const ShaderSetupDefinitionInfo& info);
        void (*BindShaderSetup)(void* rn, ShaderSetup setup);

        //texture
        void* (*CreateTexture)(const TextureCreationInfo& info);
        void (*DestroyTexture)(void* tex);

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

        //shader storage
        void* (*CreateStorageBuffer)(const StorageBufferCreationInfo& info);
        void (*DestroyStorageBuffer)(void* handle);
        void (*UploadStorageBuffer)(const StorageBufferUploadInfo& info);

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
        void (*SetVSync)(void* rn, VsyncMode mode);
    };
}

#endif //HEX_RENDERER_H
