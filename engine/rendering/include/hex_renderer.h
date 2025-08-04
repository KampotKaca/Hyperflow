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

    struct BufferUploadInfo_i
    {
        const BufferUploadPacketInfo* pUploadPackets{};
        uint32_t uploadPacketCount{};
        const uint8_t* pUniformDataBuffer{};
    };

    struct ShaderLibraryPreRasterModuleInfo_i
    {
        const char* vertexShaderCode{};
        uint32_t vertexShaderCodeSize{};

        const char* tessellationControlShaderCode{};
        uint32_t tessellationControlShaderCodeSize{};

        const char* tessellationEvaluationShaderCode{};
        uint32_t tessellationEvaluationShaderCodeSize{};

        const char* geometryShaderCode{};
        uint32_t geometryShaderCodeSize{};

        ShaderRasterizerOptions options{};
        ShaderLayout layout{};
    };

    struct ShaderLibraryFragmentModuleInfo_i
    {
        const char* fragmentShaderCode{};
        uint32_t fragmentShaderCodeSize{};
        ShaderDepthStencilOptions depthStencilOptions{};
        ShaderLayout layout{};
    };

    struct ShaderLibraryCreationInfo_i
    {
        const char* uniqueLibraryName{};
        ShaderDrawOutputFormats outputFormats{};

        ShaderLibraryVertexInputModuleInfo* pVertexInputModules{};
        uint32_t vertexInputModuleCount{};
        ShaderLibraryPreRasterModuleInfo_i* pPreRasterModules{};
        uint32_t preRasterModuleCount{};
        ShaderLibraryFragmentModuleInfo_i* pFragmentModules{};
        uint32_t fragmentModuleCount{};
        ShaderLibraryFragmentOutputModuleInfo* pFragmentOutputModules{};
        uint32_t fragmentOutputModuleCount{};
    };

    struct ShaderCreationInfo_i
    {
        ShaderLayout layout{};
        const void* library{};
        ShaderModulesInfo modules{};
    };

    struct RendererInternalFunctions_i
    {
        uint32_t (*createVulkanSurfaceFunc)(void* windowHandle, void* instance, const void* allocator, void* surfaceResult){};

        bool (*fileExistsFunc)(const char* path){};
        bool (*readFileFunc)(const std::string& filename, std::vector<char>& result){};
        bool (*writeFileFunc)(const std::string& filename, const std::vector<char>& result){};

        void* (*allocateFunc)(std::size_t n);
        void* (*allocateAlignedFunc)(std::size_t n, std::align_val_t align);
        void (*deallocateFunc)(void* p);
        void (*deallocateAlignedFunc)(void* p, std::align_val_t align);
        void* (*reallocateFunc)(void* p, std::size_t n);
    };

    struct RendererLoadInfo_i
    {
        uvec3 appVersion{};
        uvec3 engineVersion{};
        const char* applicationTitle{};
        void* platformDll{};
        RendererInternalFunctions_i functions{};
    };

    struct VertBufferUploadInfo_i
    {
        const void* buffer{};
        const void* data{};
        uint32_t offset{};
        uint32_t vertexCount{};
    };

    struct IndexBufferUploadInfo_i
    {
        const void* buffer{};
        const void* data{};
        uint32_t offset{};
        uint32_t indexCount{};
    };

    struct RendererInstanceCreationInfo_i
    {
        void* handle{};
        uvec2 size{};
        VsyncMode vSyncMode = VsyncMode::NoSync;
        void (*initCallback)();
        void (*shutdownCallback)();
    };

    struct TextureCreationInfo_i
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

    struct TexturePackTextureUploadInfo_i
    {
        struct TextureInfo
        {
            void* texture{};
            uint32_t index{};
        };

        TextureInfo* pTextures{};
        uint32_t offset = 0;
        uint32_t count = 0;
    };

    struct TexturePackBindingUploadInfo_i
    {
        uint32_t bindingIndex = 0;
        std::optional<TextureSampler> sampler{};
        std::optional<TexturePackTextureUploadInfo_i> texInfo{};
    };

    struct TexturePackBindingUploadGroupInfo_i
    {
        TexturePackBindingUploadInfo_i* bindings{};
        uint32_t bindingCount = 0;
    };

    struct TexturePackBindInfo_i
    {
        struct TextureInfo
        {
            void* texture{};
            uint32_t index{};
        };

        TexturePackBindingType type{};
        TextureSampler sampler{};
        TextureInfo* textures{};
        uint32_t arraySize{};
        uint32_t bindingIndex{};
    };

    struct TexturePackCreationInfo_i
    {
        TextureLayout layout = 0;
        uint32_t bindingCount = 0;
        TexturePackBindInfo_i* pBindings{};
    };

    template<typename T>
    struct BindResourceInfo_i
    {
        RenderBindingType bindingType = RenderBindingType::Graphics;
        uint32_t setBindingIndex = 0;
        T objects[MAX_OBJECT_BINDINGS]{};
        uint32_t objectCount = 0;
    };

    struct StorageBufferBindingInfo_i
    {
        const void* storageBuffer{};
        uint32_t setBindingIndex = 0;
    };

    struct DrawCallInfo_i
    {
        void** pVertBuffers{};
        uint32_t bufferCount{};

        void* indexBuffer{};
        uint32_t instanceCount{};
    };

    struct RendererAPI
    {
        //Creation
        void (*Load)(const RendererLoadInfo_i& info);
        void (*Unload)();
        void* (*CreateInstance)(const RendererInstanceCreationInfo_i& info);
        void (*DestroyInstance)(void* rn);

        //Shader Library
        void* (*CreateShaderLibrary)(const ShaderLibraryCreationInfo_i& info);
        void (*DestroyShaderLibrary)(void* library);

        //Shaders
        void* (*CreateShader)(const ShaderCreationInfo_i& info);
        void (*DestroyShader)(void* shader);
        void (*BindShader)(const void* rn, const void* shader);

        //shader setup
        ShaderLayout (*DefineShaderLayout)(const ShaderLayoutDefinitionInfo& info);
        void (*BindShaderLayout)(void* rn, ShaderLayout setup);
        void (*UploadPushConstants)(void* rn, const PushConstantUploadInfo& info);

        //texture
        void* (*CreateTexture)(const TextureCreationInfo_i& info);
        void (*DestroyTexture)(void* tex);

        //render texture
        void* (*CreateRenderTexture)(const RenderTextureCreationInfo& info);
        void (*DestroyRenderTexture)(void* tex);

        //texture pack
        void* (*CreateTexturePack)(const TexturePackCreationInfo_i& info);
        void (*DestroyTexturePack)(void* texPack);
        void (*UploadTexturePackBinding)(void* texPack, const TexturePackBindingUploadGroupInfo_i& info);
        void (*BindTexturePack)(void* rn, const BindResourceInfo_i<void*>& info);

        //texture sampler
        TextureSampler (*DefineTextureSampler)(const TextureSamplerDefinitionInfo& info);

        //texture layout
        TextureLayout (*DefineTextureLayout)(const TextureLayoutDefinitionInfo& info);

        //buffer attribute
        VertexBufferAttribute (*DefineVertBufferAttrib)(const VertexBufferAttributeDefinitionInfo& info, uint32_t fullStride);
        uint32_t (*GetVertBufferAttribSize)(VertexBufferAttribute attrib);

        //buffers
        Buffer (*DefineUniformBuffer)(const BufferDefinitionInfo& info);
        Buffer (*DefineStorageBuffer)(const StorageBufferDefinitionInfo& info);
        void (*UploadBuffer)(const void* rn, const BufferUploadInfo_i& info);
        void (*BindBuffer)(const void* rn, const BindResourceInfo_i<Buffer>& info);

        //vertex buffer
        void* (*CreateVertBuffer)(const VertBufferCreationInfo& info);
        void (*DestroyVertBuffer)(void* handle);
        void (*UploadVertBuffer)(const void* rn, const VertBufferUploadInfo_i& info);

        //index buffer
        void* (*CreateIndexBuffer)(const IndexBufferCreationInfo& info);
        void (*DestroyIndexBuffer)(void* handle);
        void (*UploadIndexBuffer)(const void* rn, const IndexBufferUploadInfo_i& info);

        //copy operations
        void (*SubmitBufferCopyOperations)();
        void (*SubmitTextureCopyOperations)();

        //RenderingOperations
        void (*WaitForPreviousFrame)(void* rn);
        uvec2 (*GetReadyForRendering)(void* rn, void** pTextures, uint32_t textureCount);
        void (*StartFrame)(void* rn);
        void (*EndFrame)(void* rn);
        void (*Draw)(void* rn, const DrawCallInfo_i& info);
        void (*ApplyRenderAttachmentDependencies)(void* rn, RenderAttachmentDependencyInfo* pInfos, uint32_t count);
        void (*WaitForDevice)();

        void (*BeginRendering)(void* rn, void* tex);
        void (*EndRendering)(void* rn);

        void (*RegisterFrameBufferChange)(void* rn, uvec2 newSize);
        void (*SetVSync)(void* rn, VsyncMode mode);
        void* (*GetCmd)(void* rn);

        void* (*GetEditorInfo)();
    };
}

#endif //HEX_RENDERER_H
