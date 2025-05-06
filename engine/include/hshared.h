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
#include <map>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <ranges>
#include <algorithm>

#include <bitset>
#include <queue>
#include <optional>

extern "C"
{
#include "log.h"
};

#include <sys/stat.h>
#include <sys/types.h>
#include <memory>
#include <atomic>

#include "hmath.h"

#if PLATFORM_LINUX
#ifdef None
#undef None
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

	template<typename T>
	struct DllApi
	{
		void* handle;
		T api;
	};

#define TO_RES_PATH(x) (std::string(RES_PATH) + (x))

#if PLATFORM_WINDOWS
#define API extern "C" __declspec(dllexport)
#elif PLATFORM_LINUX
#define API extern "C" __attribute__((visibility("default")))
#endif
	//endregion

	//region Definitions

	enum class Key : uint8_t
	{
		None,
		Backspace,
		Tab,
		Enter,
		Pause,
		CapsLock,
		Escape,
		Space,
		PageUp, PageDown,
		End, Home,
		Left, Up, Right, Down,
		PrintScreen,
		Insert,
		Delete,
		Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
		LeftSuper, RightSuper,
		Pad0, Pad1, Pad2, Pad3, Pad4, Pad5, Pad6, Pad7, Pad8, Pad9,
		PadMultiply,
		PadAdd,
		PadEqual,
		PadSubtract,
		PadDecimal,
		PadDivide,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
		LeftBracket,
		Backslash,
		RightBracket,
		Apostrophe,
		NumLock,
		ScrollLock,
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		LeftShift, RightShift,
		LeftControl, RightControl,
		LeftAlt, RightAlt,
		Semicolon,
		Equal,
		Comma,
		Minus,
		Period,
		Slash,
		Grave,
		Count
	};

	enum class Button : uint8_t { None, Left, Right, Wheel, Extra1, Extra2, Count };

	enum class KeyState { None, Down, DownContinues, Up };
	typedef KeyState ButtonState;

	struct Rect
	{
		vec2 position;
		vec2 size;
	};

	struct IRect
	{
		ivec2 position;
		ivec2 size;
	};

	//endregion

	//region Rendering

	struct Renderer;
	struct Shader;
	struct VertBuffer;
	struct IndexBuffer;
	struct TexturePack;
	typedef uint32_t BufferAttrib;
	typedef uint32_t UniformBuffer;
	typedef uint32_t UniformStorage;
	typedef uint32_t UniformAllocator;
	typedef uint32_t TextureSampler;

	enum class BufferDataType { U8, I8, U16, I16, U32, I32, U64, I64, F16, F32, F64, Count };
	enum class BufferMemoryType { Static, WriteOnly, ReadWrite, Count };

	constexpr uint32_t BUFFER_DATA_SIZE[(uint32_t)BufferDataType::Count] = { 1, 1, 2, 2, 4, 4, 8, 8, 2, 4, 8 };

	struct BufferAttribFormat
	{
		BufferDataType type = BufferDataType::F32;
		uint32_t size = 1;
		//Do not implement lSize
		uint32_t lSize = 1;
	};

	struct BufferAttribDefinitionInfo
	{
		uint32_t bindingId = 0;
		uint32_t formatCount = 0;
		BufferAttribFormat* pFormats = nullptr;
	};

	struct VertBufferCreationInfo
	{
		BufferAttrib bufferAttrib = 0;
		BufferMemoryType memoryType = BufferMemoryType::Static;
		uint32_t vertexCount = 0;
		void* pVertices = nullptr;
	};

	struct IndexBufferCreationInfo
	{
		BufferDataType indexFormat = BufferDataType::U16;
		BufferMemoryType memoryType = BufferMemoryType::Static;
		uint32_t indexCount = 0;
		void* pIndices = nullptr;
	};

	struct VertBufferUploadInfo
	{
		const Ref<VertBuffer>& buffer;
		const void* data;

		//data alignment will be size of the vertex, so offset should be set as how many vertices should be skipped.
		uint32_t offset;
		uint32_t vertCount;
	};

	struct IndexBufferUploadInfo
	{
		const Ref<IndexBuffer>& buffer;
		const void* data;
		uint32_t offset;
		uint32_t indexCount;
	};

	struct ShaderCreationInfo
	{
		UniformStorage uniformStorage{};
		uint32_t supportedAttribCount{};
		const BufferAttrib* pSupportedAttribs{};
		const char* vertexShaderLoc{};
		const char* fragmentShaderLoc{};
	};

	enum class UniformBufferStage
	{
		None = 0,
		Vertex = (1u << 0), TessellationControl = (1u << 1), TessellationEvaluation = (1u << 2),
		Geometry = (1u << 3), Fragment = (1u << 4), Compute = (1u << 5),
		AllGraphics = Vertex | TessellationControl | TessellationEvaluation | Geometry | Fragment,
		All = Vertex | TessellationControl | TessellationEvaluation | Geometry | Fragment | Compute,
	};

	inline UniformBufferStage operator|(UniformBufferStage a, UniformBufferStage b)
	{
		return (UniformBufferStage)((uint32_t)a | (uint32_t)b);
	}

	inline UniformBufferStage operator&(UniformBufferStage a, UniformBufferStage b)
	{
		return (UniformBufferStage)((uint32_t)a & (uint32_t)b);
	}

	inline UniformBufferStage& operator|=(UniformBufferStage& a, UniformBufferStage b)
	{
		a = a | b;
		return a;
	}

	inline UniformBufferStage& operator&=(UniformBufferStage& a, UniformBufferStage b)
	{
		a = a & b;
		return a;
	}

	//scalar alignment 4, vec2 -> 8 vec3 and up -> 16
	struct UniformBufferDefinitionInfo
	{
		uint32_t bindingId{};
		UniformBufferStage usageStageFlags{};

		//this variable describes this specific uniform buffers array size,
		//example:
		//layout(binding = 0) uniform Camera{} CAMERA[4], in this case you set arraySize to 4
		uint32_t arraySize{};

		//size of each array element
		uint32_t elementSizeInBytes{};
	};

	enum class UniformBufferBindingType { Graphics = 0, Compute = 1, RayTracing = 1000165000, HuaweiSubpassShading = 1000369003 };

	struct UniformBufferUpload
	{
		UniformBuffer buffer;
		uint32_t offsetInBytes;
		uint32_t sizeInBytes;
		const void* data;
	};

	struct UniformBufferUploadInfo
	{
		UniformBufferBindingType bindingType = UniformBufferBindingType::Graphics;
		UniformBufferUpload* pUploads{};
		uint32_t uploadCount{};
	};

	struct UniformStorageDefinitionInfo
	{
		UniformBuffer* pBuffers;
		uint32_t bufferCount;
	};

	struct UniformAllocatorDefinitionInfo
	{
		UniformBuffer* pBuffers;
		uint32_t bufferCount;
	};

	enum class TextureFormat
	{
	    Undefined = 0,
	    R4G4_Unorm_Pack8 = 1,
	    R4G4B4A4_Unorm_Pack16 = 2,
	    B4G4R4A4_Unorm_Pack16 = 3,
	    R5G6B5_Unorm_Pack16 = 4,
	    B5G6R5_Unorm_Pack16 = 5,
	    R5G5B5A1_Unorm_Pack16 = 6,
	    B5G5R5A1_Unorm_Pack16 = 7,
	    A1R5G5B5_Unorm_Pack16 = 8,
	    R8_Unorm = 9,
	    R8_Snorm = 10,
	    R8_Uscaled = 11,
	    R8_Sscaled = 12,
	    R8_Uint = 13,
	    R8_Sint = 14,
	    R8_Srgb = 15,
	    R8G8_Unorm = 16,
	    R8G8_Snorm = 17,
	    R8G8_Uscaled = 18,
	    R8G8_Sscaled = 19,
	    R8G8_Uint = 20,
	    R8G8_Sint = 21,
	    R8G8_Srgb = 22,
	    R8G8B8_Unorm = 23,
	    R8G8B8_Snorm = 24,
	    R8G8B8_Uscaled = 25,
	    R8G8B8_Sscaled = 26,
	    R8G8B8_Uint = 27,
	    R8G8B8_Sint = 28,
	    R8G8B8_Srgb = 29,
	    B8G8R8_Unorm = 30,
	    B8G8R8_Snorm = 31,
	    B8G8R8_Uscaled = 32,
	    B8G8R8_Sscaled = 33,
	    B8G8R8_Uint = 34,
	    B8G8R8_Sint = 35,
	    B8G8R8_Srgb = 36,
	    R8G8B8A8_Unorm = 37,
	    R8G8B8A8_Snorm = 38,
	    R8G8B8A8_Uscaled = 39,
	    R8G8B8A8_Sscaled = 40,
	    R8G8B8A8_Uint = 41,
	    R8G8B8A8_Sint = 42,
	    R8G8B8A8_Srgb = 43,
	    B8G8R8A8_Unorm = 44,
	    B8G8R8A8_Snorm = 45,
	    B8G8R8A8_Uscaled = 46,
	    B8G8R8A8_Sscaled = 47,
	    B8G8R8A8_Uint = 48,
	    B8G8R8A8_Sint = 49,
	    B8G8R8A8_Srgb = 50,
	    A8B8G8R8_Unorm_Pack32 = 51,
	    A8B8G8R8_Snorm_Pack32 = 52,
	    A8B8G8R8_Uscaled_Pack32 = 53,
	    A8B8G8R8_Sscaled_Pack32 = 54,
	    A8B8G8R8_Uint_Pack32 = 55,
	    A8B8G8R8_Sint_Pack32 = 56,
	    A8B8G8R8_Srgb_Pack32 = 57,
	    A2R10G10B10_Unorm_Pack32 = 58,
	    A2R10G10B10_Snorm_Pack32 = 59,
	    A2R10G10B10_Uscaled_Pack32 = 60,
	    A2R10G10B10_Sscaled_Pack32 = 61,
	    A2R10G10B10_Uint_Pack32 = 62,
	    A2R10G10B10_Sint_Pack32 = 63,
	    A2B10G10R10_Unorm_Pack32 = 64,
	    A2B10G10R10_Snorm_Pack32 = 65,
	    A2B10G10R10_Uscaled_Pack32 = 66,
	    A2B10G10R10_Sscaled_Pack32 = 67,
	    A2B10G10R10_Uint_Pack32 = 68,
	    A2B10G10R10_Sint_Pack32 = 69,
	    R16_Unorm = 70,
	    R16_Snorm = 71,
	    R16_Uscaled = 72,
	    R16_Sscaled = 73,
	    R16_Uint = 74,
	    R16_Sint = 75,
	    R16_Sfloat = 76,
	    R16G16_Unorm = 77,
	    R16G16_Snorm = 78,
	    R16G16_Uscaled = 79,
	    R16G16_Sscaled = 80,
	    R16G16_Uint = 81,
	    R16G16_Sint = 82,
	    R16G16_Sfloat = 83,
	    R16G16B16_Unorm = 84,
	    R16G16B16_Snorm = 85,
	    R16G16B16_Uscaled = 86,
	    R16G16B16_Sscaled = 87,
	    R16G16B16_Uint = 88,
	    R16G16B16_Sint = 89,
	    R16G16B16_Sfloat = 90,
	    R16G16B16A16_Unorm = 91,
	    R16G16B16A16_Snorm = 92,
	    R16G16B16A16_Uscaled = 93,
	    R16G16B16A16_Sscaled = 94,
	    R16G16B16A16_Uint = 95,
	    R16G16B16A16_Sint = 96,
	    R16G16B16A16_Sfloat = 97,
	    R32_Uint = 98,
	    R32_Sint = 99,
	    R32_Sfloat = 100,
	    R32G32_Uint = 101,
	    R32G32_Sint = 102,
	    R32G32_Sfloat = 103,
	    R32G32B32_Uint = 104,
	    R32G32B32_Sint = 105,
	    R32G32B32_Sfloat = 106,
	    R32G32B32A32_Uint = 107,
	    R32G32B32A32_Sint = 108,
	    R32G32B32A32_Sfloat = 109,
	    R64_Uint = 110,
	    R64_Sint = 111,
	    R64_Sfloat = 112,
	    R64G64_Uint = 113,
	    R64G64_Sint = 114,
	    R64G64_Sfloat = 115,
	    R64G64B64_Uint = 116,
	    R64G64B64_Sint = 117,
	    R64G64B64_Sfloat = 118,
	    R64G64B64A64_Uint = 119,
	    R64G64B64A64_Sint = 120,
	    R64G64B64A64_Sfloat = 121,
	    B10G11R11_Ufloat_Pack32 = 122,
	    E5B9G9R9_Ufloat_Pack32 = 123,
	    D16_Unorm = 124,
	    X8_D24_Unorm_Pack32 = 125,
	    D32_Sfloat = 126,
	    S8_Uint = 127,
	    D16_Unorm_S8_Uint = 128,
	    D24_Unorm_S8_Uint = 129,
	    D32_Sfloat_S8_Uint = 130,
	    BC1_RGB_Unorm_Block = 131,
	    BC1_RGB_Srgb_Block = 132,
	    BC1_RGBA_Unorm_Block = 133,
	    BC1_RGBA_Srgb_Block = 134,
	    BC2_Unorm_Block = 135,
	    BC2_Srgb_Block = 136,
	    BC3_Unorm_Block = 137,
	    BC3_Srgb_Block = 138,
	    BC4_Unorm_Block = 139,
	    BC4_Snorm_Block = 140,
	    BC5_Unorm_Block = 141,
	    BC5_Snorm_Block = 142,
	    BC6H_Ufloat_Block = 143,
	    BC6H_Sfloat_Block = 144,
	    BC7_Unorm_Block = 145,
	    BC7_Srgb_Block = 146,
	    ETC2_R8G8B8_Unorm_Block = 147,
	    ETC2_R8G8B8_Srgb_Block = 148,
	    ETC2_R8G8B8A1_Unorm_Block = 149,
	    ETC2_R8G8B8A1_Srgb_Block = 150,
	    ETC2_R8G8B8A8_Unorm_Block = 151,
	    ETC2_R8G8B8A8_Srgb_Block = 152,
	    EAC_R11_Unorm_Block = 153,
	    EAC_R11_Snorm_Block = 154,
	    EAC_R11G11_Unorm_Block = 155,
	    EAC_R11G11_Snorm_Block = 156,
	    ASTC_4x4_Unorm_Block = 157,
	    ASTC_4x4_Srgb_Block = 158,
	    ASTC_5x4_Unorm_Block = 159,
	    ASTC_5x4_Srgb_Block = 160,
	    ASTC_5x5_Unorm_Block = 161,
	    ASTC_5x5_Srgb_Block = 162,
	    ASTC_6x5_Unorm_Block = 163,
	    ASTC_6x5_Srgb_Block = 164,
	    ASTC_6x6_Unorm_Block = 165,
	    ASTC_6x6_Srgb_Block = 166,
	    ASTC_8x5_Unorm_Block = 167,
	    ASTC_8x5_Srgb_Block = 168,
	    ASTC_8x6_Unorm_Block = 169,
	    ASTC_8x6_Srgb_Block = 170,
	    ASTC_8x8_Unorm_Block = 171,
	    ASTC_8x8_Srgb_Block = 172,
	    ASTC_10x5_Unorm_Block = 173,
	    ASTC_10x5_Srgb_Block = 174,
	    ASTC_10x6_Unorm_Block = 175,
	    ASTC_10x6_Srgb_Block = 176,
	    ASTC_10x8_Unorm_Block = 177,
	    ASTC_10x8_Srgb_Block = 178,
	    ASTC_10x10_Unorm_Block = 179,
	    ASTC_10x10_Srgb_Block = 180,
	    ASTC_12x10_Unorm_Block = 181,
	    ASTC_12x10_Srgb_Block = 182,
	    ASTC_12x12_Unorm_Block = 183,
	    ASTC_12x12_Srgb_Block = 184,
	    G8B8G8R8_422_Unorm = 1000156000,
	    B8G8R8G8_422_Unorm = 1000156001,
	    G8_B8_R8_3Plane_420_Unorm = 1000156002,
	    G8_B8R8_2Plane_420_Unorm = 1000156003,
	    G8_B8_R8_3Plane_422_Unorm = 1000156004,
	    G8_B8R8_2Plane_422_Unorm = 1000156005,
	    G8_B8_R8_3Plane_444_Unorm = 1000156006,
	    R10X6_Unorm_Pack16 = 1000156007,
	    R10X6G10X6_Unorm_2Pack16 = 1000156008,
	    R10X6G10X6B10X6A10X6_Unorm_4Pack16 = 1000156009,
	    G10X6B10X6G10X6R10X6_422_Unorm_4Pack16 = 1000156010,
	    B10X6G10X6R10X6G10X6_422_Unorm_4Pack16 = 1000156011,
	    G10X6_B10X6_R10X6_3Plane_420_Unorm_3Pack16 = 1000156012,
	    G10X6_B10X6R10X6_2Plane_420_Unorm_3Pack16 = 1000156013,
	    G10X6_B10X6_R10X6_3Plane_422_Unorm_3Pack16 = 1000156014,
	    G10X6_B10X6R10X6_2Plane_422_Unorm_3Pack16 = 1000156015,
	    G10X6_B10X6_R10X6_3Plane_444_Unorm_3Pack16 = 1000156016,
	    R12X4_Unorm_Pack16 = 1000156017,
	    R12X4G12X4_Unorm_2Pack16 = 1000156018,
	    R12X4G12X4B12X4A12X4_Unorm_4Pack16 = 1000156019,
	    G12X4B12X4G12X4R12X4_422_Unorm_4Pack16 = 1000156020,
	    B12X4G12X4R12X4G12X4_422_Unorm_4Pack16 = 1000156021,
	    G12X4_B12X4_R12X4_3Plane_420_Unorm_3Pack16 = 1000156022,
	    G12X4_B12X4R12X4_2Plane_420_Unorm_3Pack16 = 1000156023,
	    G12X4_B12X4_R12X4_3Plane_422_Unorm_3Pack16 = 1000156024,
	    G12X4_B12X4R12X4_2Plane_422_Unorm_3Pack16 = 1000156025,
	    G12X4_B12X4_R12X4_3Plane_444_Unorm_3Pack16 = 1000156026,
	    G16B16G16R16_422_Unorm = 1000156027,
	    B16G16R16G16_422_Unorm = 1000156028,
	    G16_B16_R16_3Plane_420_Unorm = 1000156029,
	    G16_B16R16_2Plane_420_Unorm = 1000156030,
	    G16_B16_R16_3Plane_422_Unorm = 1000156031,
	    G16_B16R16_2Plane_422_Unorm = 1000156032,
	    G16_B16_R16_3Plane_444_Unorm = 1000156033,
	    G8_B8R8_2Plane_444_Unorm = 1000330000,
	    G10X6_B10X6R10X6_2Plane_444_Unorm_3Pack16 = 1000330001,
	    G12X4_B12X4R12X4_2Plane_444_Unorm_3Pack16 = 1000330002,
	    G16_B16R16_2Plane_444_Unorm = 1000330003,
	    A4R4G4B4_Unorm_Pack16 = 1000340000,
	    A4B4G4R4_Unorm_Pack16 = 1000340001,
	    ASTC_4x4_Sfloat_Block = 1000066000,
	    ASTC_5x4_Sfloat_Block = 1000066001,
	    ASTC_5x5_Sfloat_Block = 1000066002,
	    ASTC_6x5_Sfloat_Block = 1000066003,
	    ASTC_6x6_Sfloat_Block = 1000066004,
	    ASTC_8x5_Sfloat_Block = 1000066005,
	    ASTC_8x6_Sfloat_Block = 1000066006,
	    ASTC_8x8_Sfloat_Block = 1000066007,
	    ASTC_10x5_Sfloat_Block = 1000066008,
	    ASTC_10x6_Sfloat_Block = 1000066009,
	    ASTC_10x8_Sfloat_Block = 1000066010,
	    ASTC_10x10_Sfloat_Block = 1000066011,
	    ASTC_12x10_Sfloat_Block = 1000066012,
	    ASTC_12x12_Sfloat_Block = 1000066013,
	    A1B5G5R5_Unorm_Pack16 = 1000470000,
	    A8_Unorm = 1000470001,
	};

	enum class TextureType   	{ Tex1D = 0, Tex2D = 1, Tex3D = 2 };
	enum class TextureChannel   { Default = 0, Gray = 1, GrayAlpha = 2, RGB = 3, RGBA = 4 };

	enum class TextureFilter			{ Point = 0, Bilinear = 1 };
	enum class TextureAnisotropicFilter	{ None, X2 = 2, X4 = 4, X8 = 8, X16 = 16, X32 = 32 };
	enum class TextureRepeatMode		{ Repeat = 0, MirroredRepeat = 1, ClampToEdge = 2, ClampToBorder = 3, MirrorClampToEdge = 4 };
	enum class ComparisonOperation		{ None = 0, Never = 1, Less = 2, Equal = 3, LessOrEqual = 4, Greater = 5, NotEqual = 6, GreaterOrEqual = 7, Always = 8 };

	struct TextureSamplerDefinitionInfo
	{
		TextureFilter filter = TextureFilter::Bilinear;
		TextureAnisotropicFilter anisotropicFilter = TextureAnisotropicFilter::X8;
		TextureRepeatMode repeatMode = TextureRepeatMode::Repeat;
		bool useNormalizedCoordinates = true;
		ComparisonOperation comparison = ComparisonOperation::None;
	};

	struct TextureCreationInfo
	{
		const char* filePath{};
		TextureFormat format = TextureFormat::R8G8B8A8_Srgb;
		TextureChannel desiredChannel = TextureChannel::RGBA;
		TextureSampler sampler{};
	};

	struct TexturePackCreationInfo
	{
		TextureCreationInfo* pTextures{};
		uint32_t textureCount{};
		TextureType type = TextureType::Tex2D;
		BufferMemoryType memoryType = BufferMemoryType::Static;
	};

	enum class RenderingApiType { None, Vulkan, Direct3D };

	struct DrawCallInfo
	{
		Ref<VertBuffer>* pVertBuffers; //vertex buffers to render
		uint32_t bufferCount = 0; //amount of buffers to render

		Ref<IndexBuffer> indexBuffer; //is optional property, if nullptr engine will render vertex buffer in ordered manner.
		uint32_t instanceCount = 0; //amount of instances to render
	};

	//endregion

	//region Window

	enum class WindowStyle
	{
		Default
	};

	enum class WindowFlags
	{
		Visible   = 1 << 0,
		Minimized = 1 << 1,
		Maximized = 1 << 2,

		Default = Visible
	};

	struct WindowData
	{
		std::string title = "Untitled";
		WindowFlags flags = WindowFlags::Default;
		WindowStyle style = WindowStyle::Default;
		ivec2 position = ivec2{ 300, 300 };
		ivec2 size = ivec2{ 200, 200 };

		void (*onPreRenderCallback)(const Ref<Renderer>&){};
		void (*onRenderCallback)(const Ref<Renderer>&){};
	};

	//endregion

	//region Engine

	enum class EngineUpdateType
	{
		Continues,   //Updates as often as is allowed, good for games or simulations, try to avoid it if you it's possible
		EventRaised  //Waits updates only on OS events, good for apps which doesn't change without user interaction
	};

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

	struct EngineData
	{
		std::string appTitle = "Hyperflow";
		RenderingApiType renderingApi = RenderingApiType::Vulkan; // type of initial api, can be changed later
		EngineUpdateType updateType = EngineUpdateType::EventRaised; // type of application updates
		EngineLifecycleCallbacks lifecycleCallbacks{}; //passed engine callbacks to interact with the engine
		WindowData windowData{}; //properties of the initial window
	};

	struct Window;

	//endregion

	//region Helpers

	uint32_t TrailingZeros64(uint64_t n);

	//endregion
}

#endif //HSHARED_H
