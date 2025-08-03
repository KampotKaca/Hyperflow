#ifndef HSHARED_H
#define HSHARED_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cstdarg>
#include <vector>
#include <set>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <ranges>
#include <algorithm>

#include <bitset>
#include <queue>
#include <optional>
#include <filesystem>

extern "C"
{
#include "log.h"
};

#include <sys/stat.h>
#include <sys/types.h>
#include <memory>
#include <atomic>

#include "phmap/btree.h"
#include "phmap/phmap.h"

#include "henums.h"
#include "hmath.h"
#include "hinput.h"
#include "../config.h"

#if PLATFORM_LINUX
#ifdef None
#undef None
#endif

#ifdef Always
#undef Always
#endif

#endif

namespace hf
{
	//region Aditionalls
	typedef void (*ActionCallback)(void* params);

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T>
	using URef = std::unique_ptr<T>;

	template <typename T, typename... Args>
	Ref<T> MakeRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template <typename T, typename... Args>
	URef<T> MakeURef(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template <class K, class V>
	using unordered_map = phmap::flat_hash_map<K, V>;

#define TO_RES_PATH(x) (std::filesystem::current_path() / (x)).string()

#if PLATFORM_WINDOWS
#define API extern "C" __declspec(dllexport)
#elif PLATFORM_LINUX
#define API extern "C" __attribute__((visibility("default")))
#endif

	//endregion

	//region Definitions

    struct FilePath
    {
        std::string path{};
        bool isAbsolute = false;
    };

	//endregion

	//region Rendering

	struct Renderer;
	struct ShaderLibrary;
	struct Shader;
	struct Texture;
	struct Cubemap;
	struct TexturePack;

	struct RuntimeBufferBase;
	struct VertBuffer;
	struct IndexBuffer;
	struct Mesh;
	struct Material;
	struct RenderTexture;

	typedef uint32_t BufferAttrib;
	typedef uint32_t Buffer;
	typedef uint32_t TextureLayout;
	typedef uint32_t TextureSampler;
	typedef uint32_t ShaderLayout;

	struct BufferAttribFormat
	{
		BufferDataType type = BufferDataType::F32;
		uint32_t size = 1;
		//Do not implement lSize
		uint32_t lSize = 1;
	};

	struct BufferAttribDefinitionInfo
	{
	    BufferInputRate inputRate = BufferInputRate::Vertex;
		uint32_t bindingId = 0;
		uint32_t formatCount = 0;
		BufferAttribFormat* pFormats = nullptr;
	};

	struct VertBufferCreationInfo
	{
		BufferAttrib bufferAttrib = 0;
		BufferMemoryType memoryType = BufferMemoryType::Static;
		BufferUsageType usageFlags = BufferUsageType::Vertex;
		uint32_t vertexCount = 0;
		void* pVertices = nullptr;
	};

	struct IndexBufferCreationInfo
	{
		BufferDataType indexFormat = BufferDataType::U16;
		BufferMemoryType memoryType = BufferMemoryType::Static;
		BufferUsageType usageFlags = BufferUsageType::Index;
		uint32_t indexCount = 0;
		void* pIndices = nullptr;
	};

	struct VertBufferUploadInfo
	{
		const void* data{};

		//data alignment will be size of the vertex, so offset should be set as how many vertices should be skipped.
		uint32_t offset{};
		uint32_t vertCount{};
	};

	struct IndexBufferUploadInfo
	{
		const void* data{};
		uint32_t offset{};
		uint32_t indexCount{};
	};

    struct BlendingOptions
    {
        ColorBlendFactorType srcFactor = ColorBlendFactorType::SrcAlpha;
        ColorBlendFactorType dstFactor = ColorBlendFactorType::OneMinusSrcAlpha;
        ColorBlendOp op = ColorBlendOp::Add;
    };

	struct ShaderBlendingOptions
	{
	    BlendingOptions colorBlendingOptions{};
	    BlendingOptions alphaBlendingOptions
	    {
	        .srcFactor = ColorBlendFactorType::One,
            .dstFactor = ColorBlendFactorType::Zero,
	    };
	};

    struct ShaderDepthBiasOptions
    {
        float constantFactor = 0.0f;
        float clamp = 0.0f;
        float slopeFactor = 0.0f;
    };

	struct ShaderRasterizerOptions
	{
		ShaderCullMode cullMode = ShaderCullMode::Back;
		ShaderFaceDirection faceDirection = ShaderFaceDirection::Clockwise;
	    MeshPolygonMode polygonMode = MeshPolygonMode::Fill;
	    bool enableRasterizerDiscard = false;
	    bool enableDepthClamping = false;
	    std::optional<ShaderDepthBiasOptions> biasOptions{};
	    float lineWidth = 1.0f; //Width of the line in Topology Lines mode.
	};

	struct ShaderDepthStencilOptions
	{
		bool enableDepth = true;
		bool writeDepth = true;
		DepthComparisonFunction comparisonFunc = DepthComparisonFunction::LessOrEqual;
		bool enableDepthBounds = false;
		bool enableStencil = false;

		StencilOperation frontStencil = StencilOperation::Keep;
		StencilOperation backStencil = StencilOperation::Zero;

		vec2 depthBounds = { 0, 1 };
	};

    struct ShaderDrawOutputFormats
    {
        MultisampleMode sampleMode = MultisampleMode::MSAA_1X;
        uint32_t colorFormatCount = 1;
        TextureFormat colorFormats[MAX_COLOR_ATTACHMENTS] = { VULKAN_API_COLOR_FORMAT };
        TextureFormat depthFormat = TextureFormat::D32_Sfloat;
        TextureFormat stencilFormat = TextureFormat::Undefined;
    };

    struct ColorAttachmentSettings
    {
        ColorMaskingFlags colorWriteMask = ColorMaskingFlags::All;
        std::optional<ShaderBlendingOptions> blending{};
    };

    template<typename T>
    struct ShaderLibraryModule
    {
        uint32_t* resultId{};
        T module{};
    };

    struct ShaderLibraryVertexInputModuleInfo
    {
        BufferAttrib pAttributes[MAX_VERTEX_INPUT_BUFFER_ATTRIBUTES]{};
        uint32_t attributeCount{};
        MeshPrimitiveTopologyType topology = MeshPrimitiveTopologyType::TriangleList;
        bool enablePrimitiveRestart = false;
    };

    struct ShaderLibraryPreRasterModuleInfo
    {
        FilePath vertexShaderPath{};
        std::optional<FilePath> tessellationControlShaderPath{};
        std::optional<FilePath> tessellationEvaluationShaderPath{};
        std::optional<FilePath> geometryShaderPath{};
        ShaderRasterizerOptions options{};
        ShaderLayout layout{};
    };

    struct ShaderLibraryFragmentModuleInfo
    {
        FilePath fragmentShaderPath{};
        ShaderDepthStencilOptions depthStencilOptions{};
        ShaderLayout layout{};
    };

    struct ShaderLibraryFragmentOutputModuleInfo
    {
        ColorAttachmentSettings pColorAttachmentsSettings[MAX_COLOR_ATTACHMENTS]{};
        uint32_t colorAttachmentCount = 1;
		std::optional<ShaderBlendOp> blendOp;
    };

    struct ShaderLibraryCreationInfo
    {
        std::string uniqueLibraryName{};
        ShaderDrawOutputFormats outputFormats{};

        ShaderLibraryModule<ShaderLibraryVertexInputModuleInfo>* pVertexInputModules{};
        uint32_t vertexInputModuleCount{};
        ShaderLibraryModule<ShaderLibraryPreRasterModuleInfo>* pPreRasterModules{};
        uint32_t preRasterModuleCount{};
        ShaderLibraryModule<ShaderLibraryFragmentModuleInfo>* pFragmentModules{};
        uint32_t fragmentModuleCount{};
        ShaderLibraryModule<ShaderLibraryFragmentOutputModuleInfo>* pFragmentOutputModules{};
        uint32_t fragmentOutputModuleCount{};
    };

    struct ShaderModulesInfo
    {
        uint32_t vertexInputModuleId{};
        uint32_t preRasterModuleId{};
        uint32_t fragmentModuleId{};
        uint32_t fragmentOutputModuleId{};
    };

	struct ShaderCreationInfo
	{
		ShaderLayout layout{};
	    Ref<ShaderLibrary> library{};
	    ShaderModulesInfo modules{};
	};

	struct BufferBindingInfo
	{
		ShaderUsageStage usageFlags = ShaderUsageStage::Vertex | ShaderUsageStage::Fragment;

		//this variable describes this specific uniform buffers array size,
		//example:
		//layout(binding = 0) uniform Camera{} CAMERA[4], in this case you set arraySize to 4
		uint32_t arraySize{};

		//size of each array element
		uint32_t elementSizeInBytes{};
	};

	//scalar alignment 4, vec2 -> 8 vec3 and up -> 16
	struct BufferDefinitionInfo
	{
		uint32_t bindingId{};
		BufferBindingInfo* pBindings{};
		uint32_t bindingCount{};
	};

	struct StorageBufferDefinitionInfo
	{
		BufferDefinitionInfo bufferInfo;
		BufferMemoryType memoryType = BufferMemoryType::Static;
		const uint8_t* data{};
	};

	struct BufferUploadInfo
	{
		Buffer buffer{};
		uint32_t offsetInBytes{};
		uint32_t sizeInBytes{};
		const void* data{};
	};

	struct TextureLayoutBindingInfo
	{
		uint32_t bindingId{};
		ShaderUsageStage usageFlags = ShaderUsageStage::Vertex | ShaderUsageStage::Fragment;

		//this variable describes this specific uniform buffers array size,
		//example:
		//layout(binding = 0) uniform Camera{} CAMERA[4], in this case you set arraySize to 4
		uint32_t arraySize{};
	};

	struct TextureLayoutDefinitionInfo
	{
		TextureLayoutBindingInfo* pBindings{};
		uint32_t bindingCount{};
	};

	//quick access and upload buffer.
	//at minimum it's size should be 4 bytes.
	//remember first 2 bytes are always occupied by material index, so you can use other 126 bytes freely
	//on vulkan it directly translates to push constant, but on other pipelines ...
	struct PushConstantInfo
	{
		ShaderUsageStage usageFlags = ShaderUsageStage::None;
		uint32_t sizeInBytes{};
	};

	struct PushConstantUploadInfo
	{
		ShaderLayout layout{};
		const void* data{};
	};

	struct ShaderLayoutDefinitionInfo
	{
		PushConstantInfo pushConstant{};

		Buffer* pBuffers{};
		uint32_t bufferCount{};

		TextureLayout* pTextureLayouts{};
		uint32_t textureLayoutCount{};
	};

	struct TextureMipMapInfo
	{
		MipMapMode mode = MipMapMode::Linear;
		float_t minLod = 0.0f;
		float_t maxLod = 4.0f;
		float_t lodBias = 0.0f;
	};

	struct TextureSamplerDefinitionInfo
	{
		TextureFilter filter = TextureFilter::Bilinear;
		TextureAnisotropicFilter anisotropicFilter = TextureAnisotropicFilter::X16;
		TextureRepeatMode repeatMode = TextureRepeatMode::Repeat;
		bool useNormalizedCoordinates = true;
		ComparisonOperation comparison = ComparisonOperation::None;
		TextureMipMapInfo mipMaps{};
	};

	struct TextureDetails
	{
		TextureFormat format = TextureFormat::R8G8B8A8_Srgb;
		TextureAspectFlags aspectFlags = TextureAspectFlags::Color;
		TextureTiling tiling = TextureTiling::Optimal;
		TextureUsageFlags usageFlags = TextureUsageFlags::Sampled;
		BufferMemoryType memoryType = BufferMemoryType::Static;
		TextureResultLayoutType finalLayout = TextureResultLayoutType::ShaderReadOnly;
	};

	struct TextureCreationInfo
	{
		const char* filePath{};
		bool useAbsolutePath{};
		TextureChannel desiredChannel = TextureChannel::RGBA;
		uint32_t mipLevels = 1;
		TextureDetails details{};
	};

	struct CubemapTexturePaths
	{
		const char* left{};
		const char* right{};
		const char* down{};
		const char* up{};
		const char* back{};
		const char* front{};
	};

	struct CubemapCreationInfo
	{
		const char* folderPath{};
		TextureChannel desiredChannel = TextureChannel::RGBA;
		uint32_t mipLevels = 1;
		CubemapTexturePaths texturePaths{};
		TextureDetails details{};
	};

    template<typename T>
    struct TexturePackBindingInfo
    {
        struct TextureInfo
        {
            Ref<T> texture{};
            uint32_t index{};
        };

        TextureSampler sampler{};
        TextureInfo* textures{};
        uint32_t arraySize{};
        uint32_t bindingIndex{};
    };

	struct TexturePackCreationInfo
	{
		TexturePackBindingInfo<Texture>* pTextureBindings{};
		uint32_t textureBindingCount = 0;
		TexturePackBindingInfo<Cubemap>* pCubemapBindings{};
		uint32_t cubemapBindingCount = 0;
	    TexturePackBindingInfo<RenderTexture>* pRenderTextureBindings{};
	    uint32_t renderTextureBindingCount = 0;
		TextureLayout layout = 0;
	};

	template<typename T>
	struct TexturePackTextureUploadInfo
	{
        struct TextureInfo
        {
            Ref<T> texture{};
            uint32_t index{}; //since every texture can be an array this is an index of the texture in the array.
        };

		const TextureInfo* pTextures{};
		uint32_t count = 0;
		uint32_t offset = 0;
	};

	template<typename T>
	struct TexturePackBindingUploadInfo
	{
		uint32_t bindingIndex = 0;
		std::optional<TextureSampler> sampler{};
		std::optional<TexturePackTextureUploadInfo<T>> texInfo{};
	};

	struct RenderTextureColorAttachmentInfo
	{
		//type of attachment layout color, depth, stencil etc.
		TextureResultLayoutType layout = TextureResultLayoutType::Color;
		TextureFormat format = TextureFormat::R8G8B8A8_Snorm;
		LoadStoreOperationType lsOperation = LoadStoreOperationType::ClearAndStore;
	    TextureUsageFlags usageFlags = TextureUsageFlags::None;
	    //If used render texture will not create view for the attachment,
	    //this is fine when you are using it for presentation, but will cause problems if you use for any other purpouse
	    bool isUsedForPresentation = false;
		vec4 clearColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	};

	struct RenderTextureDepthAttachmentInfo
	{
		//type of attachment layout color, depth, stencil etc.
		TextureResultLayoutType layout = TextureResultLayoutType::DepthStencil;
		LoadStoreOperationType lsOperation = LoadStoreOperationType::ClearAndStore;
	    DepthStencilMode mode = DepthStencilMode::Depth;
        TextureUsageFlags usageFlags = TextureUsageFlags::None;

		float_t clearDepth = 1.0f;
		uint8_t clearStencil = 0;
	};

	struct RenderAttachmentDependencyTarget
	{
		RenderPipelineStage stageMask = RenderPipelineStage::None;
		AccessType accessMask = AccessType::None;
	    TextureResultLayoutType targetLayout = TextureResultLayoutType::Undefined;
	};

	struct RenderAttachmentDependencyInfo
	{
		uint32_t attachmentIndex = 0;
	    TextureAspectFlags aspectFlags = TextureAspectFlags::Color;

		RenderAttachmentDependencyTarget src
		{
			.stageMask = RenderPipelineStage::ColorAttachmentOutput,
			.accessMask = AccessType::None,
		    .targetLayout = TextureResultLayoutType::Undefined
		};
		RenderAttachmentDependencyTarget dst
		{
			.stageMask = RenderPipelineStage::ColorAttachmentOutput,
			.accessMask = AccessType::ColorAttachmentWrite,
		    .targetLayout = TextureResultLayoutType::Attachment
		};
	};

    //All color attachments are first, then presentation attachment and lastly depth attachment
    //So when passing dependencies use attachment indexes according to that rule.
    struct RenderTextureCreationInfo
    {
        RenderTextureColorAttachmentInfo pColorAttachments[MAX_COLOR_ATTACHMENTS]{};
        uint32_t colorAttachmentCount = 0;

        std::optional<RenderTextureDepthAttachmentInfo> depthAttachment{};

        MultisampleMode multisampleMode = MultisampleMode::MSAA_1X;
        uvec2 size{};
        uvec2 offset{};
    };

	struct MaterialCreationInfo
	{
		uint32_t sizeInBytes{};
	};

	struct MeshStats
	{
		MeshDataType typeFlags = MeshDataType::Default;
		BufferMemoryType memoryType = BufferMemoryType::Static;
		BufferAttrib bufferAttrib{};
	};

	struct MeshCreationInfo
	{
		const char* filePath{};
		MeshStats stats{};
	};

	struct DrawCallInfo
	{
		Ref<VertBuffer>* pVertBuffers{}; //vertex buffers to render
		uint32_t bufferCount = 0; //amount of buffers to render

		Ref<IndexBuffer> indexBuffer{}; //is optional property, if nullptr engine will render vertex buffer in ordered manner.
		uint32_t instanceCount = 0; //amount of instances to render

#if DEBUG
		char debugName[16];
#endif
	};

	//endregion

	//region Window

	struct Window;

	struct RendererEventInfo
	{
		void (*onRendererInitCallback)(){};
		void (*onRendererShutdownCallback)(){};

		void (*onPreRenderCallback)(const Ref<Renderer>&){};
		void (*onRenderCallback)(const Ref<Renderer>&){};
	};

	struct WindowCreationInfo
	{
		std::string title = "Untitled";
		const char* iconFolderPath{};
		WindowEventFlags eventFlags = WindowEventFlags::All;
		WindowPointerState pointerState = WindowPointerState::Normal;
		WindowStyle style = WindowStyle::Default;
		ivec2 position = ivec2{ 300, 300 };
		ivec2 size = ivec2{ 200, 200 };
		VsyncMode vsyncMode = VsyncMode::Relaxed;

		RendererEventInfo rnEventInfo{};
	};

	//endregion

	//region Engine

	struct EngineLifecycleCallbacks
	{
		//Called after new renderer is loaded, this is where you should define uin32_t type rendering objects
		//like: uniform buffer, buffer attribute and ect...
		void (*onRendererLoad)(){};
		//Called when initializing platform and renderer is completed.
		void (*onResourcesLoad)(){};
		//Called until the first frame update
		void (*onStartCallback)(){};
		//Called every frame
		void (*onUpdateCallback)(){};
		//Called when application starts releasing resources until it terminates
		void (*onQuitCallback)(){};
	};

    struct EngineInternalResourceFormatInfo
    {
		BufferBindingInfo globalUniformBindingInfo{}; //this is binding for global uniform which should contain at least Camera and Time uniforms
        ShaderDrawOutputFormats drawOutputFormats{}; //this is general outline of the render texture you are going to draw on with the shaders.
    };

    struct EngineInternalAudioInfo
    {
        float_t volume = 1.0f; //Initial global audio volume multiplier
        uint32_t usedListenersCount = 1; //Set this to 0 and audio system will not be initialized.
    };

	struct EngineData
	{
		std::string appTitle = "Hyperflow";
		RenderingApiType renderingApi = RenderingApiType::Vulkan; // type of initial api, can be changed later
		EngineUpdateType updateType = EngineUpdateType::EventRaised; // type of application updates
	    EngineInternalResourceFormatInfo internalResourcesFormat{}; //format of internal resources which are used by the engine
	    EngineLifecycleCallbacks lifecycleCallbacks{}; //passed engine callbacks to interact with the engine
	    EngineInternalAudioInfo audioInfo{}; //audio info which is used by the engine
	    WindowCreationInfo windowData{}; //properties of the initial window
	};

	//endregion

	//region Global Uniforms

	struct CameraUniformInfo
	{
		alignas(16) vec3 lookDirection;
		alignas(16) vec3 position;
		alignas(16) mat4 view;
		alignas(16) mat4 invView;
		alignas(16) mat4 proj;
		alignas(16) mat4 invProj;
		alignas(16) mat4 viewProj;
	};

	struct TimeUniformInfo
	{
		alignas(8) double_t deltaTime;
		alignas(8) double_t timeSinceStartup;
	};

	struct LightUniformInfo
	{
		struct alignas(16) Directional
		{
			alignas(16) vec3 direction;
			alignas(16) vec3 color;
		};

		struct alignas(16) Spot
		{
			alignas(16) vec3 position;
			alignas(16) vec3 direction;
			alignas(16) vec3 color;
			alignas(16) vec3 rangeStats; //x innerAngle, y outerAngle, z range
		};

		struct alignas(16) Point
		{
			alignas(16) vec3 position;
			alignas(16) vec4 color; // w is range
		};

		alignas(16) Directional directionalLights[MAX_DIRECTIONAL_LIGHTS]{};
		alignas(16) Spot spotLights[MAX_SPOT_LIGHTS]{};
		alignas(16) Point pointLights[MAX_POINT_LIGHTS]{};

		alignas(16) uvec3 lightCounts{}; //x directional, y spot, z point
	};

	//endregion

	//region Constant Objects

	struct GridLinesInfo
	{
		alignas(16) float_t lineThickness = 3.0f;
		alignas(16) vec4 color{ 0.2f, 0.2f, 0.2f, 0.4f };
	};

	struct SkyboxInfo
	{

	};

	//endregion

	//region Audio

	struct AudioClip;
	struct AudioPlayer;
	struct AudioPlayer3D;
	struct AudioListener;

    struct AudioCone
    {
        float_t innerAngle = 360.0f;
        float_t outerAngle = 360.0f;
        float_t outerGain = 0.1f;

        vec3 position{};
        vec3 euler{};
    };

	struct AudioClipSettings
	{
		AudioClipFormat format = AudioClipFormat::Default;
		uint32_t sampleRate = 0;
		AudioClipChannelMixMode channelMixMode = AudioClipChannelMixMode::Rectangular;
		AudioClipDitherMode ditherMode = AudioClipDitherMode::None;
		AudioClipEncodingFormat encodingFormat = AudioClipEncodingFormat::Unknown;
	};

	struct AudioClipCreationInfo
	{
		const char* filePath{};
		bool useAbsolutePath = false;
		AudioClipSettings settings{};
	};

	struct AudioPlayerSettings
	{
		float_t volume = 1.0f;
		float_t pitch = 1.0f;
		bool loopingEnabled = false;
	};

    struct AudioPlayer3DSettings
    {
        float_t maxRange = 100.0f;
        float_t falloff = 10.0f;
        Audio3DAttenuationModel attenuationModel = Audio3DAttenuationModel::Linear;
    };

	struct AudioPlayerCreationInfo
	{
		Ref<AudioClip> clip{};
		AudioPlayerSettings settings{};
	};

    struct AudioPlayer3DCreationInfo
    {
        Ref<AudioClip> clip{};
        AudioPlayerSettings settings{};
        AudioPlayer3DSettings settings3d{};
        AudioCone cone{};
    };

    struct AudioListenerCreationInfo
    {
        bool isEnabled = true;
        AudioCone cone{};
    };

	//endregion

    struct GlobalMemoryStatistics
    {
        double mappedSizeMbs = 0;
        double mappedPeakSizeMbs = 0;
        double cachedSizeMbs = 0;
        double hugeAllocSizeMbs = 0;
        double hugeAllocPeakSizeMbs = 0;
        double mappedTotalSizeMbs = 0;
        double unmappedTotalSizeMbs = 0;
    };

    struct ThreadMemoryStatistics
    {
        double cacheSizeMbs = 0;
        double cacheSpanMbs = 0;
        double threadToGlobalMbs = 0;
        double globalToThreadMbs = 0;

        size_t currentNumSpans = 0;
        size_t peakNumSpans = 0;

        size_t currentNumAllocations = 0;
        size_t peakNumAllocations = 0;
        size_t totalNumAllocations = 0;
        size_t totalNumFrees = 0;
    };

	namespace utils
	{
		inline uint32_t GetFirstBitOne64(const uint64_t n) { return n ? __builtin_ctzll(n) : 64u; }
		inline uint32_t GetFirstBitZero64(const uint64_t n) { return ~n ? __builtin_ctzll(~n) : 64u; }

		constexpr vec3 ColorFromHash(const uint32_t colorHash)
		{
			const float_t r = (float_t)((colorHash >> 16) & 0xFF) / 255.0f;
			const float_t g = (float_t)((colorHash >> 8) & 0xFF) / 255.0f;
			const float_t b = (float_t)(colorHash & 0xFF) / 255.0f;
			return { r, g, b };
		}
	}
}

#endif //HSHARED_H
