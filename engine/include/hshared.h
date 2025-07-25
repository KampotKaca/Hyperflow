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

#define DEFINE_ENUM_FLAGS(Enum)\
	inline Enum operator|(Enum a, Enum b) { return (Enum)((uint32_t)a | (uint32_t)b); }\
	inline Enum operator&(Enum a, Enum b) { return (Enum)((uint32_t)a & (uint32_t)b); }\
	inline Enum operator^(Enum a, Enum b) { return (Enum)((uint32_t)a ^ (uint32_t)b); }\
	inline Enum& operator|=(Enum& a, Enum b)\
	{\
		a = a | b;\
		return a;\
	}\
	inline Enum& operator&=(Enum& a, Enum b)\
	{\
		a = a & b;\
		return a;\
	}\

	//endregion

	//region Definitions

	enum class DataTransferType { DoNotOwn, TransferOwnership, CopyData };
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
	struct TexturePackAllocator;

	struct RuntimeBufferBase;
	struct VertBuffer;
	struct IndexBuffer;
	struct Mesh;
	struct Material;
	struct RenderTexture;

	typedef uint32_t BufferAttrib;
	typedef uint32_t Buffer;
	typedef uint32_t BufferAllocator;
	typedef uint32_t TextureLayout;
	typedef uint32_t TextureSampler;
	typedef uint32_t ShaderLayout;

    enum class TexturePackBindingType { Texture2D, Cubemap, RenderTexture };
	enum class CubemapTextureType { Left, Right, Down, Up, Back, Front };

	enum class BufferDataType { U8, I8, U16, I16, U32, I32, U64, I64, F16, F32, F64, Count };
	enum class BufferMemoryType { Static, WriteOnly, PerFrameWriteOnly, ReadWrite, PerFrameReadWrite, Count };
	enum class RenderBindingType { Graphics = 0, Compute = 1, RayTracing = 1000165000, HuaweiSubpassShading = 1000369003 };

	enum class AccessType
	{
		None = 0,
		IndirectCommandRead = (1 << 0),
		IndexRead = (1 << 1), VertAttribRead = (1 << 2),
		UniformRead = (1 << 3), AttachmentRead = (1 << 4),
		ShaderRead = (1 << 5), ShaderWrite = (1 << 6),
		ColorAttachmentRead = (1 << 7), ColorAttachmentWrite = (1 << 8),
		DepthStencilAttachmentRead = (1 << 9), DepthStencilAttachmentWrite	= (1 << 10),
		TransferRead = (1 << 11), TransferWrite = (1 << 12),
		HostRead = (1 << 13), HostWrite = (1 << 14),
		MemoryRead = (1 << 15), MemoryWrite = (1 << 16),
	};
	DEFINE_ENUM_FLAGS(AccessType)

	enum class ShaderUsageStage
	{
		None = 0,
		Vertex = (1u << 0), TessellationControl = (1u << 1), TessellationEvaluation = (1u << 2),
		Geometry = (1u << 3), Fragment = (1u << 4), Compute = (1u << 5),
		Default = Vertex | Fragment,
		AllGraphics = Vertex | TessellationControl | TessellationEvaluation | Geometry | Fragment,
		All = Vertex | TessellationControl | TessellationEvaluation | Geometry | Fragment | Compute,
	};
	DEFINE_ENUM_FLAGS(ShaderUsageStage)

	enum class RenderPipelineStage
	{
		None = 0,
		PreDraw = (1 << 0),
		DrawIndirect = (1 << 1), VertexInput = (1 << 2), Vertex = (1 << 3),
		TessellationControl = (1 << 4), TessellationEvaluation = (1 << 5),
		Geometry = (1 << 6), Fragment = (1 << 7),
		EarlyFragmentTest = (1 << 8), LateFragmentTest = (1 << 9), ColorAttachmentOutput = (1 << 10),
		Compute = (1 << 11), Transfer = (1 << 12),
		PostDraw = (1 << 13),
		Host = (1 << 14), AllGraphics = (1 << 15), AllCommands = (1 << 16),
		Default = Vertex | Fragment,
	};
	DEFINE_ENUM_FLAGS(RenderPipelineStage)

	constexpr uint32_t BUFFER_DATA_SIZE[(uint32_t)BufferDataType::Count] = { 1, 1, 2, 2, 4, 4, 8, 8, 2, 4, 8 };

	enum class BufferUsageType
	{
		TransferSrc = 1 << 0, TransferDst = 1 << 1, UniformTexel = 1 << 2, StorageTexel = 1 << 3,
		Uniform = 1 << 4, Storage = 1 << 5, Index = 1 << 6, Vertex = 1 << 7, Indirect = 1 << 8,
		Default = TransferSrc | TransferDst | Vertex | Index | Storage,
		All = TransferSrc | TransferDst | UniformTexel | StorageTexel | Uniform | Storage | Index | Vertex | Indirect,
	};
	DEFINE_ENUM_FLAGS(BufferUsageType)

	enum class ShaderBlendMode { None = 0, Alpha = 1, Logical = 2 };
	enum class ShaderCullMode { None = 0, Front = 1, Back = 2, Both = 3 };
	enum class ShaderFaceDirection { CounterClockwise = 0, Clockwise = 1 };

	enum class ShaderBlendOp
	{
		Clear, And, AndReverse, Copy, AndInverted, NoOp, XOr, Or, Nor,
		Equivalent, Invert, OrReverse, CopyInverted, OrInverted, Nand, Set
	};

	enum class DepthComparisonFunction
	{
		Never, Less, Equal, LessOrEqual, Greater,
		NotEqual, GreaterOrEqual, Always
	};

	enum class StencilOperation
	{
		Keep, Zero, Replace, IncrementAndClamp, DecrementAndClamp,
		Invert, IncrementAndWrap, DecrementAndWrap,
	};

    enum class TextureFormat
	{
	    Undefined = 0,
	    R4G4_Unorm_Pack8 = 1, R4G4B4A4_Unorm_Pack16 = 2, B4G4R4A4_Unorm_Pack16 = 3, R5G6B5_Unorm_Pack16 = 4,
	    B5G6R5_Unorm_Pack16 = 5, R5G5B5A1_Unorm_Pack16 = 6, B5G5R5A1_Unorm_Pack16 = 7, A1R5G5B5_Unorm_Pack16 = 8,
	    R8_Unorm = 9, R8_Snorm = 10, R8_Uscaled = 11, R8_Sscaled = 12, R8_Uint = 13, R8_Sint = 14, R8_Srgb = 15,
	    R8G8_Unorm = 16, R8G8_Snorm = 17, R8G8_Uscaled = 18, R8G8_Sscaled = 19,
	    R8G8_Uint = 20, R8G8_Sint = 21, R8G8_Srgb = 22, R8G8B8_Unorm = 23, R8G8B8_Snorm = 24,
	    R8G8B8_Uscaled = 25, R8G8B8_Sscaled = 26, R8G8B8_Uint = 27, R8G8B8_Sint = 28,
	    R8G8B8_Srgb = 29, B8G8R8_Unorm = 30, B8G8R8_Snorm = 31, B8G8R8_Uscaled = 32, B8G8R8_Sscaled = 33,
	    B8G8R8_Uint = 34, B8G8R8_Sint = 35, B8G8R8_Srgb = 36, R8G8B8A8_Unorm = 37, R8G8B8A8_Snorm = 38,
	    R8G8B8A8_Uscaled = 39, R8G8B8A8_Sscaled = 40, R8G8B8A8_Uint = 41, R8G8B8A8_Sint = 42,
	    R8G8B8A8_Srgb = 43, B8G8R8A8_Unorm = 44, B8G8R8A8_Snorm = 45, B8G8R8A8_Uscaled = 46, B8G8R8A8_Sscaled = 47,
	    B8G8R8A8_Uint = 48, B8G8R8A8_Sint = 49, B8G8R8A8_Srgb = 50,
	    A8B8G8R8_Unorm_Pack32 = 51, A8B8G8R8_Snorm_Pack32 = 52, A8B8G8R8_Uscaled_Pack32 = 53, A8B8G8R8_Sscaled_Pack32 = 54,
	    A8B8G8R8_Uint_Pack32 = 55, A8B8G8R8_Sint_Pack32 = 56, A8B8G8R8_Srgb_Pack32 = 57,
	    A2R10G10B10_Unorm_Pack32 = 58, A2R10G10B10_Snorm_Pack32 = 59, A2R10G10B10_Uscaled_Pack32 = 60, A2R10G10B10_Sscaled_Pack32 = 61,
	    A2R10G10B10_Uint_Pack32 = 62, A2R10G10B10_Sint_Pack32 = 63, A2B10G10R10_Unorm_Pack32 = 64, A2B10G10R10_Snorm_Pack32 = 65,
	    A2B10G10R10_Uscaled_Pack32 = 66, A2B10G10R10_Sscaled_Pack32 = 67, A2B10G10R10_Uint_Pack32 = 68, A2B10G10R10_Sint_Pack32 = 69,
	    R16_Unorm = 70, R16_Snorm = 71, R16_Uscaled = 72, R16_Sscaled = 73, R16_Uint = 74, R16_Sint = 75, R16_Sfloat = 76,
	    R16G16_Unorm = 77, R16G16_Snorm = 78, R16G16_Uscaled = 79, R16G16_Sscaled = 80, R16G16_Uint = 81, R16G16_Sint = 82, R16G16_Sfloat = 83,
	    R16G16B16_Unorm = 84, R16G16B16_Snorm = 85, R16G16B16_Uscaled = 86, R16G16B16_Sscaled = 87,
	    R16G16B16_Uint = 88, R16G16B16_Sint = 89, R16G16B16_Sfloat = 90,
	    R16G16B16A16_Unorm = 91, R16G16B16A16_Snorm = 92, R16G16B16A16_Uscaled = 93, R16G16B16A16_Sscaled = 94,
	    R16G16B16A16_Uint = 95, R16G16B16A16_Sint = 96, R16G16B16A16_Sfloat = 97,
	    R32_Uint = 98, R32_Sint = 99, R32_Sfloat = 100,
	    R32G32_Uint = 101, R32G32_Sint = 102, R32G32_Sfloat = 103,
	    R32G32B32_Uint = 104, R32G32B32_Sint = 105, R32G32B32_Sfloat = 106,
	    R32G32B32A32_Uint = 107, R32G32B32A32_Sint = 108, R32G32B32A32_Sfloat = 109,
	    R64_Uint = 110, R64_Sint = 111, R64_Sfloat = 112,
	    R64G64_Uint = 113, R64G64_Sint = 114, R64G64_Sfloat = 115,
	    R64G64B64_Uint = 116, R64G64B64_Sint = 117, R64G64B64_Sfloat = 118,
	    R64G64B64A64_Uint = 119, R64G64B64A64_Sint = 120, R64G64B64A64_Sfloat = 121,
	    B10G11R11_Ufloat_Pack32 = 122, E5B9G9R9_Ufloat_Pack32 = 123, D16_Unorm = 124, X8_D24_Unorm_Pack32 = 125,
	    D32_Sfloat = 126, S8_Uint = 127, D16_Unorm_S8_Uint = 128, D24_Unorm_S8_Uint = 129, D32_Sfloat_S8_Uint = 130,
	    BC1_RGB_Unorm_Block = 131, BC1_RGB_Srgb_Block = 132, BC1_RGBA_Unorm_Block = 133, BC1_RGBA_Srgb_Block = 134,
	    BC2_Unorm_Block = 135, BC2_Srgb_Block = 136, BC3_Unorm_Block = 137, BC3_Srgb_Block = 138,
	    BC4_Unorm_Block = 139, BC4_Snorm_Block = 140, BC5_Unorm_Block = 141, BC5_Snorm_Block = 142,
	    BC6H_Ufloat_Block = 143, BC6H_Sfloat_Block = 144, BC7_Unorm_Block = 145, BC7_Srgb_Block = 146,
	    ETC2_R8G8B8_Unorm_Block = 147, ETC2_R8G8B8_Srgb_Block = 148,
	    ETC2_R8G8B8A1_Unorm_Block = 149, ETC2_R8G8B8A1_Srgb_Block = 150, ETC2_R8G8B8A8_Unorm_Block = 151, ETC2_R8G8B8A8_Srgb_Block = 152,
	    EAC_R11_Unorm_Block = 153, EAC_R11_Snorm_Block = 154,
	    EAC_R11G11_Unorm_Block = 155, EAC_R11G11_Snorm_Block = 156,
	    ASTC_4x4_Unorm_Block = 157, ASTC_4x4_Srgb_Block = 158, ASTC_5x4_Unorm_Block = 159, ASTC_5x4_Srgb_Block = 160,
	    ASTC_5x5_Unorm_Block = 161, ASTC_5x5_Srgb_Block = 162, ASTC_6x5_Unorm_Block = 163, ASTC_6x5_Srgb_Block = 164,
	    ASTC_6x6_Unorm_Block = 165, ASTC_6x6_Srgb_Block = 166, ASTC_8x5_Unorm_Block = 167, ASTC_8x5_Srgb_Block = 168,
	    ASTC_8x6_Unorm_Block = 169, ASTC_8x6_Srgb_Block = 170, ASTC_8x8_Unorm_Block = 171, ASTC_8x8_Srgb_Block = 172,
	    ASTC_10x5_Unorm_Block = 173, ASTC_10x5_Srgb_Block = 174, ASTC_10x6_Unorm_Block = 175, ASTC_10x6_Srgb_Block = 176,
	    ASTC_10x8_Unorm_Block = 177, ASTC_10x8_Srgb_Block = 178, ASTC_10x10_Unorm_Block = 179, ASTC_10x10_Srgb_Block = 180,
	    ASTC_12x10_Unorm_Block = 181, ASTC_12x10_Srgb_Block = 182, ASTC_12x12_Unorm_Block = 183, ASTC_12x12_Srgb_Block = 184,
		Count = 185,
	 //    G8B8G8R8_422_Unorm = 1000156000, B8G8R8G8_422_Unorm = 1000156001, G8_B8_R8_3Plane_420_Unorm = 1000156002,
	 //    G8_B8R8_2Plane_420_Unorm = 1000156003, G8_B8_R8_3Plane_422_Unorm = 1000156004,
	 //    G8_B8R8_2Plane_422_Unorm = 1000156005, G8_B8_R8_3Plane_444_Unorm = 1000156006,
	 //    R10X6_Unorm_Pack16 = 1000156007, R10X6G10X6_Unorm_2Pack16 = 1000156008, R10X6G10X6B10X6A10X6_Unorm_4Pack16 = 1000156009,
	 //    G10X6B10X6G10X6R10X6_422_Unorm_4Pack16 = 1000156010, B10X6G10X6R10X6G10X6_422_Unorm_4Pack16 = 1000156011,
	 //    G10X6_B10X6_R10X6_3Plane_420_Unorm_3Pack16 = 1000156012, G10X6_B10X6R10X6_2Plane_420_Unorm_3Pack16 = 1000156013,
	 //    G10X6_B10X6_R10X6_3Plane_422_Unorm_3Pack16 = 1000156014, G10X6_B10X6R10X6_2Plane_422_Unorm_3Pack16 = 1000156015, G10X6_B10X6_R10X6_3Plane_444_Unorm_3Pack16 = 1000156016,
	 //    R12X4_Unorm_Pack16 = 1000156017, R12X4G12X4_Unorm_2Pack16 = 1000156018,
	 //    R12X4G12X4B12X4A12X4_Unorm_4Pack16 = 1000156019, G12X4B12X4G12X4R12X4_422_Unorm_4Pack16 = 1000156020,
	 //    B12X4G12X4R12X4G12X4_422_Unorm_4Pack16 = 1000156021, G12X4_B12X4_R12X4_3Plane_420_Unorm_3Pack16 = 1000156022,
	 //    G12X4_B12X4R12X4_2Plane_420_Unorm_3Pack16 = 1000156023, G12X4_B12X4_R12X4_3Plane_422_Unorm_3Pack16 = 1000156024, G12X4_B12X4R12X4_2Plane_422_Unorm_3Pack16 = 1000156025,
	 //    G12X4_B12X4_R12X4_3Plane_444_Unorm_3Pack16 = 1000156026, G16B16G16R16_422_Unorm = 1000156027, B16G16R16G16_422_Unorm = 1000156028,
	 //    G16_B16_R16_3Plane_420_Unorm = 1000156029, G16_B16R16_2Plane_420_Unorm = 1000156030, G16_B16_R16_3Plane_422_Unorm = 1000156031,
	 //    G16_B16R16_2Plane_422_Unorm = 1000156032, G16_B16_R16_3Plane_444_Unorm = 1000156033, G8_B8R8_2Plane_444_Unorm = 1000330000,
	 //    G10X6_B10X6R10X6_2Plane_444_Unorm_3Pack16 = 1000330001, G12X4_B12X4R12X4_2Plane_444_Unorm_3Pack16 = 1000330002, G16_B16R16_2Plane_444_Unorm = 1000330003,
	 //    A4R4G4B4_Unorm_Pack16 = 1000340000, A4B4G4R4_Unorm_Pack16 = 1000340001, ASTC_4x4_Sfloat_Block = 1000066000,
	 //    ASTC_5x4_Sfloat_Block = 1000066001, ASTC_5x5_Sfloat_Block = 1000066002, ASTC_6x5_Sfloat_Block = 1000066003,
	 //    ASTC_6x6_Sfloat_Block = 1000066004, ASTC_8x5_Sfloat_Block = 1000066005, ASTC_8x6_Sfloat_Block = 1000066006,
	 //    ASTC_8x8_Sfloat_Block = 1000066007, ASTC_10x5_Sfloat_Block = 1000066008, ASTC_10x6_Sfloat_Block = 1000066009,
		// ASTC_10x8_Sfloat_Block = 1000066010, ASTC_10x10_Sfloat_Block = 1000066011, ASTC_12x10_Sfloat_Block = 1000066012,
	 //    ASTC_12x12_Sfloat_Block = 1000066013, A1B5G5R5_Unorm_Pack16 = 1000470000, A8_Unorm = 1000470001,
	};

    enum class MultisampleMode { MSAA_1X = 1, MSAA_2X = 2, MSAA_4X = 4, MSAA_8X = 8 };

    enum class TextureChannel           { Default = 0, Gray = 1, GrayAlpha = 2, RGB = 3, RGBA = 4 };
    enum class TextureFilter			{ Point = 0, Bilinear = 1 };
    enum class TextureAnisotropicFilter	{ None, X2 = 2, X4 = 4, X8 = 8, X16 = 16, X32 = 32 };
    enum class TextureRepeatMode		{ Repeat = 0, MirroredRepeat = 1, ClampToEdge = 2, ClampToBorder = 3, MirrorClampToEdge = 4 };
    enum class ComparisonOperation		{ None = 0, Never = 1, Less = 2, Equal = 3, LessOrEqual = 4, Greater = 5, NotEqual = 6, GreaterOrEqual = 7, Always = 8 };
    enum class MipMapMode				{ Nearest = 0, Linear = 1 };

    enum class TextureAspectFlags
    {
        None = 0, Color = 1 << 0, Depth = 1 << 1, Stencil = 1 << 2, MetaData = 1 << 3,
        Plane0 = 1 << 4, Plane1 = 1 << 5, Plane2 = 1 << 6,
    };
    DEFINE_ENUM_FLAGS(TextureAspectFlags)

    enum class TextureResultLayoutType
    {
        Undefined = 0, General = 1,
        Color = 2, DepthStencil = 3, DepthStencilReadOnly = 4,
        //Generally used in post processing
        ShaderReadOnly = 5,
        //Used for images which are copied from
        TransferSrc = 6,
        //Used for images which are copied to
        TransferDst = 7,
        Preinitialized = 8,
        DepthReadOnly_Stencil = 1000117000,
        Depth_StencilReadOnly = 1000117001,
        Depth = 1000241000,
        DepthReadOnly = 1000241001,
        Stencil = 1000241002,
        StencilReadOnly = 1000241003,
        ReadOnly = 1000314000,
        Attachment = 1000314001,
        RenderingLocalRead = 1000232000,
    };

    enum class TextureTiling
    {
        Optimal = 0, Linear = 1
    };

    enum class TextureUsageFlags
    {
        None = 0, TransferSrc = 1 << 0, TransferDst = 1 << 1, Sampled = 1 << 2, Storage = 1 << 3,
        Color = 1 << 4, DepthStencil = 1 << 5, Transient = 1 << 6, Input = 1 << 7, Host = 1 << 22,
    };
    DEFINE_ENUM_FLAGS(TextureUsageFlags)

    enum class DepthStencilMode { None = 0, Depth = 1 << 0, Stencil = 1 << 1 };
    DEFINE_ENUM_FLAGS(DepthStencilMode)

    enum class MeshDataType
    {
        None = 0,
        Position = 1 << 0, Normal = 1 << 1, Color = 1 << 2, TexCoord = 1 << 3,
        Default = Position | TexCoord,
        All = Position | Normal | TexCoord | Color,
    };
    DEFINE_ENUM_FLAGS(MeshDataType)

    enum class MeshIndexFormat
    {
        U16 = (uint32_t)BufferDataType::U16,
        U32 = (uint32_t)BufferDataType::U32,
    };

    enum class LoadStoreOperationType
    {
        LoadAndStore = 0, ClearAndStore = 1, DontCareAndStore = 2,
        LoadAndDontCare = 3, ClearAndDontCare = 4, DontCareAndDontCare = 5,
    };

	enum class RenderingApiType { None, Vulkan, Direct3D };

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

	struct ShaderBlendingOptions
	{
		ShaderBlendMode blendMode = ShaderBlendMode::Alpha;
		ShaderBlendOp blendOp = ShaderBlendOp::XOr; //Setting will be used only if you use Logical Blending
	};

	struct ShaderRasterizerOptions
	{
		ShaderCullMode cullMode = ShaderCullMode::Back;
		ShaderFaceDirection faceDirection = ShaderFaceDirection::Clockwise;
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
        uint32_t colorFormatCount = 1;
        TextureFormat colorFormats[MAX_COLOR_ATTACHMENTS] = { VULKAN_API_COLOR_FORMAT };
        TextureFormat depthFormat = TextureFormat::D32_Sfloat;
        TextureFormat stencilFormat = TextureFormat::Undefined;
    };

    template<typename T>
    struct ShaderLibraryModule
    {
        uint32_t* resultId{};
        T module{};
    };

    struct ShaderLibraryVertexInputModuleInfo
    {
        BufferAttrib attribute{};
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
        ShaderDrawOutputFormats drawOutputFormats{};
        ShaderBlendingOptions blendingOptions{};
    };

    struct ShaderLibraryCreationInfo
    {
        MultisampleMode sampleMode = MultisampleMode::MSAA_1X;

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

	struct TexturePackAllocatorCreationInfo
	{
		Ref<TexturePack>* pTexturePacks{};
		uint32_t texturePackCount = 0;
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

	struct BufferAllocatorDefinitionInfo
	{
		Buffer* pBuffers{};
		uint32_t bufferCount{};
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
		TextureAnisotropicFilter anisotropicFilter = TextureAnisotropicFilter::X8;
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
            uint32_t index{};
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

	enum class WindowStyle
	{
		Default
	};

	enum class WindowState
	{
		None = 0, Restored = 1, Minimized = 2, Maximized = 3,
		Hidden = 4, Fullscreen = 5, FullscreenBorderless = 6,
	};

	enum class WindowEventFlags
	{
		Key = 1 << 0, Char = 1 << 1, PointerMove = 1 << 2, PointerState = 1 << 3,
		Button = 1 << 4, Scroll = 1 << 5, DragAndDrop = 1 << 6, Close = 1 << 7,
		Focus = 1 << 8, Minimize = 1 << 9, Maximize = 1 << 10,
		Move = 1 << 11, Resize = 1 << 12, Refresh = 1 << 13,
		Default = Key | Char | PointerMove | PointerState | Button | Scroll | DragAndDrop |
				  Close | Focus | Minimize | Maximize | Move | Resize,
		All = Default | Refresh,

	};
	DEFINE_ENUM_FLAGS(WindowEventFlags)

	enum class VsyncMode { NoSync, Relaxed, Full, Count };

	struct RendererEventInfo
	{
		void (*onRendererInitCallback)(){};
		void (*onRendererShutdownCallback)(){};

		void (*onPreRenderCallback)(const Ref<Renderer>&){};
		void (*onRenderCallback)(const Ref<Renderer>&){};
	};

	enum class WindowPointerState { Normal, Hidden, Disabled, Captured };

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

    struct EngineInternalResourceFormatInfo
    {
		BufferBindingInfo globalUniformBindingInfo{}; //this is binding for global uniform which should contain at least Camera and Time uniforms
        ShaderDrawOutputFormats drawOutputFormats{}; //this is general outline of the render texture you are going to draw on with the shaders.
        MultisampleMode samplingMode{}; //current multisampling mode.
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

	enum class AudioClipFormat { Default = 0, U8 = 1, S16 = 2, S24 = 3, S32 = 4, F32 };
	enum class AudioClipChannelMixMode { Rectangular = 0, Simple = 1, CustomWeights = 2 };
	enum class AudioClipDitherMode { None = 0, Rectangle = 1, Triangle = 2 };
	enum class AudioClipEncodingFormat { Unknown = 0, Wav = 1, Flac = 2, Mp3 = 3, Vorbis = 4 };
	enum class Audio3DAttenuationModel { None = 0, Inverse = 1, Linear = 2, Exponential = 3 };
	enum class AudioPlayerStateFlags { None = 0, Loaded = 1 << 0, Playing = 1 << 1 };
    DEFINE_ENUM_FLAGS(AudioPlayerStateFlags)

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

	namespace utils
	{
		inline uint32_t GetFirstBitOne64(uint64_t n) { return n ? __builtin_ctzll(n) : 64u; }
		inline uint32_t GetFirstBitZero64(uint64_t n) { return ~n ? __builtin_ctzll(~n) : 64u; }

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
