#ifndef HEX_RENDERER_H
#define HEX_RENDERER_H

#include "hshared.h"
#include "hdrawprocess.h"
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

    enum class TextureFlags
    {
        None = 0,
        SparseBinding = 1 << 0, SparseResidency = 1 << 1, SparseAliased = 1 << 2, MutableFormat = 1 << 3,
        CubeCompatible = 1 << 4, Array2DCompatible = 1 << 5, SplitInstanceBindRegions = 1 << 6, BlockTexelViewCompatible = 1 << 7,
        ExtendedUsage = 1 << 8, Disjoint = 1 << 9, Aliased = 1 << 10, Protected = 1 << 11,
    };

	enum class TextureType { Tex1D = 0, Tex2D = 1, Tex3D = 2 };
	enum class TextureViewType
	{
	    Tex1D = (uint32_t)TextureType::Tex1D, Tex2D = (uint32_t)TextureType::Tex2D, Tex3D = (uint32_t)TextureType::Tex3D,
	    TexCube = 3, Tex1DArray = 4, Tex2DArray = 5, TexCubeArray = 6
	};

    struct BufferUploadInfo
    {
        const BufferUploadPacketInfo* pUploadPackets{};
        uint32_t uploadPacketCount{};
        const uint8_t* pUniformDataBuffer{};
    };

    struct ShaderCreationInfo
    {
        ShaderSetup shaderSetup{};
        uint32_t supportedAttribCount{};
        const BufferAttrib* pSupportedAttribs{};

        const char* vCode{};
        uint32_t vCodeSize = 0;
        const char* fCode{};
        uint32_t fCodeSize = 0;

        ShaderDrawOutputFormats drawOutputFormats{};
        ShaderRasterizerOptions rasterizerOptions{};
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

    struct RendererInstanceCreationInfo
    {
        void* handle{};
        uvec2 size{};
        VsyncMode vSyncMode = VsyncMode::NoSync;
        void (*initCallback)();
        void (*shutdownCallback)();
    };

    struct TextureCreationInfo
    {
		TextureType type = TextureType::Tex2D;
		TextureViewType viewType = TextureViewType::Tex2D;
        TextureFlags flags = TextureFlags::None;
        uvec3 size = { 1, 1, 1 };
        TextureChannel channel = TextureChannel::RGBA;
        uint32_t mipLevels = 1;
        MultisampleMode samples = MultisampleMode::MSAA_1X;
        const void* pTextures{};
        uint32_t textureCount = 1;
        TextureDetails details{};
    };

    struct TexturePackTextureUploadInfo
    {
        void** pTextures{};
        uint32_t offset = 0;
        uint32_t count = 0;
    };

    struct TexturePackBindingCreationInfo
    {
        TextureSampler sampler{};
        void** pTextures{};
        uint32_t count = 0;
    };

    struct TexturePackBindingUploadInfo
    {
        uint32_t bindingIndex = 0;
        std::optional<TextureSampler> sampler{};
        std::optional<TexturePackTextureUploadInfo> texInfo{};
    };

    struct TexturePackBindingUploadGroupInfo
    {
        TexturePackBindingUploadInfo* bindings{};
        uint32_t bindingCount = 0;
    };

    struct TexturePackCreationInfo
    {
        RenderBindingType bindingType = RenderBindingType::Graphics;
        uint32_t bindingId = 0;
        TexturePackBindingCreationInfo* pBindings{};
        uint32_t bindingCount = 0;
        TextureLayout layout = 0;
    };

    struct TexturePackAllocatorCreationInfo
    {
        void** pTexturePacks{};
        uint32_t texturePackCount = 0;
    };

    struct TexturePackBindingInfo
    {
        const void* texturePack{};
        uint32_t setBindingIndex = 0;
    };

    struct StorageBufferBindingInfo
    {
        const void* storageBuffer{};
        uint32_t setBindingIndex = 0;
    };

    struct BufferBindInfo
    {
        RenderBindingType bindingType = RenderBindingType::Graphics;
        uint32_t setBindingIndex = 0;
        Buffer* pBuffers{};
        uint32_t bufferCount = 0;
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
        void (*DestroyInstance)(void* rn);

        //Shaders
        void* (*CreateShader)(const ShaderCreationInfo& info);
        void (*DestroyShader)(void* shader);
        void (*BindShader)(const void* rn, const ShaderBindingInfo& info);

        //shader setup
        ShaderSetup (*DefineShaderSetup)(const ShaderSetupDefinitionInfo& info);
        void (*BindShaderSetup)(void* rn, ShaderSetup setup);
        void (*UploadPushConstants)(void* rn, const PushConstantUploadInfo& info);

        //texture
        void* (*CreateTexture)(const TextureCreationInfo& info);
        void (*DestroyTexture)(void* tex);

        //render texture
        void* (*CreateRenderTexture)(const RenderTextureCreationInfo& info);
        void (*DestroyRenderTexture)(void* tex);
        void (*AttachRenderTextureToRenderer)(void* rn, void* tex);

        //texture pack
        void* (*CreateTexturePack)(const TexturePackCreationInfo& info);
        void (*DestroyTexturePack)(void* texPack);
        void (*UploadTexturePackBinding)(void* texPack, const TexturePackBindingUploadGroupInfo& info);
        void (*BindTexturePack)(void* rn, const TexturePackBindingInfo& info);

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

        //buffers
        Buffer (*DefineUniformBuffer)(const BufferDefinitionInfo& info);
        Buffer (*DefineStorageBuffer)(const StorageBufferDefinitionInfo& info);
        void (*UploadBuffer)(const void* rn, const BufferUploadInfo& info);
        void (*BindBuffer)(const void* rn, const BufferBindInfo& info);

        //uniform allocator
        BufferAllocator (*DefineBufferAllocator)(const BufferAllocatorDefinitionInfo& info);

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
        void (*WaitForPreviousFrame)(void* rn);
        bool (*GetReadyForRendering)(void* rn);
        void (*StartFrame)(void* rn);
        void (*EndFrame)(void* rn);
        void (*Draw)(void* rn, const DrawCallInfo& info);
        void (*ApplyRenderAttachmentDependencies)(void* rn, RenderAttachmentDependencyInfo* pInfos, uint32_t count);
        void (*WaitForDevice)();

        void (*BeginRendering)(void* rn);
        void (*EndRendering)(void* rn);

        void (*RegisterFrameBufferChange)(void* rn, uvec2 newSize);
        void (*SetVSync)(void* rn, VsyncMode mode);
        void* (*GetCmd)(void* rn);

        void* (*GetEditorInfo)();
    };
}

#endif //HEX_RENDERER_H
