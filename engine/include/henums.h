#ifndef HENUMS_H
#define HENUMS_H

namespace hf
{
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

    //----------------------------- Rendering ---------------------------------------------------

    enum class TexturePackBindingType { Texture2D, Cubemap, RenderTexture };
	enum class CubemapTextureType { Left, Right, Down, Up, Back, Front };

	enum class BufferDataType { U8, I8, U16, I16, U32, I32, U64, I64, F16, F32, F64, Count };
	enum class BufferMemoryType { Static, WriteOnly, PerFrameWriteOnly, ReadWrite, PerFrameReadWrite, Count };
	enum class RenderBindingType { Graphics = 0, Compute = 1, RayTracing = 1000165000, HuaweiSubpassShading = 1000369003 };

    enum class BufferInputRate { Vertex = 0, Instance = 1 };

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

	enum class ShaderCullMode { None = 0, Front = 1, Back = 2, Both = 3 };
	enum class ShaderFaceDirection { CounterClockwise = 0, Clockwise = 1 };

	enum class ShaderBlendOp
	{
		Clear, And, AndReverse, Copy, AndInverted, NoOp, XOr, Or, Nor,
		Equivalent, Invert, OrReverse, CopyInverted, OrInverted, Nand, Set
	};

    enum class ColorBlendOp
    {
        Add = 0, Subtract = 1, ReverseSubtract = 2, Min = 3, Max = 4,
        Zero = 1000148000, Src = 1000148001, Dst = 1000148002,
        SrcOver = 1000148003, DstOver = 1000148004, SrcIn = 1000148005, DstIn = 1000148006,
        SrcOut = 1000148007, DstOut = 1000148008, SrcAtop = 1000148009, DstAtop = 1000148010,
        Xor = 1000148011, Multiply = 1000148012, Screen = 1000148013, Overlay = 1000148014,
        Darken = 1000148015, Lighten = 1000148016, ColorDodge = 1000148017, ColorBurn = 1000148018,
        HardLight = 1000148019, SoftLight = 1000148020, Difference = 1000148021, Exclusion = 1000148022,
        Invert = 1000148023, InvertRGB = 1000148024, LinearDodge = 1000148025,
        LinearBurn = 1000148026, VividLight = 1000148027, LinearLight = 1000148028, PinLight = 1000148029, HardMix = 1000148030,
        HslHue = 1000148031, HslSaturation = 1000148032,
        HslColor = 1000148033, HslLuminosity = 1000148034,
        Plus = 1000148035, PlusClamped = 1000148036, PlusClampedAlpha = 1000148037, PlusDarken = 1000148038,
        Minus = 1000148039, MinusClamped = 1000148040,
        Contrast = 1000148041, InvertOVG = 1000148042,
        Red = 1000148043, Green = 1000148044, Blue = 1000148045
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

    enum class MeshPrimitiveTopologyType
    {
        PointList = 0, LineList = 1, LineStrip = 2, TriangleList = 3, TriangleStrip = 4, TriangleFan = 5,
        LineListWithAdjacency = 6,      //Used with geometry shaders only!
        LineStripWithAdjacency = 7,     //Used with geometry shaders only!
        TriangleListWithAdjacency = 8,  //Used with geometry shaders only!
        TriangleStripWithAdjacency = 9, //Used with geometry shaders only!
        PatchList = 10,                 //Used with tesselation shaders only!
    };

    enum class MeshPolygonMode
    {
        Fill = 0, Line = 1, Point = 2,
        FillRectangleNV = 1000153000,
    };

    enum class ColorMaskingFlags
    {
        Red = 1 << 0, Green = 1 << 1, Blue = 1 << 2, Alpha = 1 << 3,
        All = Red | Green | Blue | Alpha,
    };
    DEFINE_ENUM_FLAGS(ColorMaskingFlags)

    enum class ColorBlendFactorType
    {
        Zero = 0, One = 1, SrcColor = 2, OneMinusSrcColor = 3,
        DstColor = 4, OneMinusDstColor = 5, SrcAlpha = 6,
        OneMinusSrcAlpha = 7, DstAlpha = 8, OneMinusDstAlpha = 9,
        ConstantColor = 10, OneMinusConstantColor = 11, ConstantAlpha = 12,
        OneMinusConstantAlpha = 13, SrcAlphaSaturate = 14, Src1Color = 15,
        OneMinusSrc1Color = 16, Src1Alpha = 17, OneMinusSrc1Alpha = 18,
    };

    //----------------------------- Audio ---------------------------------------------------

    enum class AudioClipFormat { Default = 0, U8 = 1, S16 = 2, S24 = 3, S32 = 4, F32 };
    enum class AudioClipChannelMixMode { Rectangular = 0, Simple = 1, CustomWeights = 2 };
    enum class AudioClipDitherMode { None = 0, Rectangle = 1, Triangle = 2 };
    enum class AudioClipEncodingFormat { Unknown = 0, Wav = 1, Flac = 2, Mp3 = 3, Vorbis = 4 };
    enum class Audio3DAttenuationModel { None = 0, Inverse = 1, Linear = 2, Exponential = 3 };
    enum class AudioPlayerStateFlags { None = 0, Loaded = 1 << 0, Playing = 1 << 1 };
    DEFINE_ENUM_FLAGS(AudioPlayerStateFlags)

    //----------------------------- Window ---------------------------------------------------

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
	enum class WindowPointerState { Normal, Hidden, Disabled, Captured };

    enum class EngineUpdateType
    {
        Continues,   //Updates as often as is allowed, good for games or simulations, try to avoid it if you it's possible
        EventRaised  //Waits updates only on OS events, good for apps which doesn't change without user interaction
    };

    //----------------------------- General ---------------------------------------------------

	enum class DataTransferType { DoNotOwn, TransferOwnership, CopyData };
}

#endif //HENUMS_H
