#ifndef HSTRCONVERSION_H
#define HSTRCONVERSION_H

#include "hshared.h"
#include "hyperflow.h"

namespace hf
{
    constexpr uint32_t fnv1a(const std::string_view str)
    {
        uint32_t hash = 2166136261u;
        for (const char c : str)
        {
            const char lower = (c >= 'A' && c <= 'Z') ? (c - 'A' + 'a') : c;
            hash ^= (uint8_t)lower;
            hash *= 16777619u;
        }
        return hash;
    }

#define DATA_TYPE(v) case fnv1a(#v): return X::v;
#define DEFAULT_TYPE(v, t)\
    default: log_warn_s("Unknown %s string: %s", #t, str);\
    return X::v;

    constexpr BufferDataType STRING_TO_BUFFER_DATA_TYPE(const std::string_view str)
    {
#define X BufferDataType
        switch (fnv1a(str))
        {
            DATA_TYPE(U8) DATA_TYPE(I8)
            DATA_TYPE(U16) DATA_TYPE(I16)
            DATA_TYPE(U32) DATA_TYPE(I32)
            DATA_TYPE(U64) DATA_TYPE(I64)
            DATA_TYPE(F16) DATA_TYPE(F32) DATA_TYPE(F64)
            DEFAULT_TYPE(F32, BufferDataType)
        }
#undef X
    }

    constexpr BufferMemoryType STRING_TO_BUFFER_MEMORY_TYPE(const std::string_view str)
    {
#define X BufferMemoryType
        switch (fnv1a(str))
        {
            DATA_TYPE(Static) DATA_TYPE(WriteOnly) DATA_TYPE(ReadWrite)
            DEFAULT_TYPE(Static, BufferMemoryType)
        }
#undef X
    }

    constexpr MeshDataType STRING_TO_MESH_DATA_TYPE(const std::string_view str)
    {
#define X MeshDataType
        switch (fnv1a(str))
        {
            DATA_TYPE(Position) DATA_TYPE(Normal) DATA_TYPE(Color) DATA_TYPE(TexCoord)
            DEFAULT_TYPE(Position, MeshDataType)
        }
#undef X
    }

    constexpr TextureChannel STRING_TO_TEXTURE_CHANNEL(const std::string_view str)
    {
#define X TextureChannel
        switch (fnv1a(str))
        {
            DATA_TYPE(Default) DATA_TYPE(Gray) DATA_TYPE(GrayAlpha) DATA_TYPE(RGB) DATA_TYPE(RGBA)
            DEFAULT_TYPE(Default, TextureChannel)
        }
#undef X
    }

    constexpr TextureAspectFlags STRING_TO_TEXTURE_ASPECT_FLAGS(const std::string_view str)
    {
#define X TextureAspectFlags
        switch (fnv1a(str))
        {
            DATA_TYPE(None) DATA_TYPE(Color) DATA_TYPE(Depth)
            DATA_TYPE(Stencil) DATA_TYPE(MetaData)
            DATA_TYPE(Plane0) DATA_TYPE(Plane1) DATA_TYPE(Plane2)
            DEFAULT_TYPE(Color, TextureAspectFlags)
        }
#undef X
    }

    constexpr TextureFormat STRING_TO_TEXTURE_FORMAT(const std::string_view str)
    {
#define X TextureFormat
        switch (fnv1a(str))
        {
            DATA_TYPE(Undefined) DATA_TYPE(R4G4_Unorm_Pack8)
            DATA_TYPE(R4G4B4A4_Unorm_Pack16) DATA_TYPE(B4G4R4A4_Unorm_Pack16)
            DATA_TYPE(R5G6B5_Unorm_Pack16) DATA_TYPE(B5G6R5_Unorm_Pack16)
            DATA_TYPE(R5G5B5A1_Unorm_Pack16) DATA_TYPE(B5G5R5A1_Unorm_Pack16) DATA_TYPE(A1R5G5B5_Unorm_Pack16)

            DATA_TYPE(R8_Unorm) DATA_TYPE(R8_Snorm)
            DATA_TYPE(R8_Uscaled) DATA_TYPE(R8_Sscaled)
            DATA_TYPE(R8_Uint) DATA_TYPE(R8_Sint) DATA_TYPE(R8_Srgb)
            DATA_TYPE(R8G8_Unorm) DATA_TYPE(R8G8_Snorm)
            DATA_TYPE(R8G8_Uscaled) DATA_TYPE(R8G8_Sscaled)

            DATA_TYPE(R8G8_Uint) DATA_TYPE(R8G8_Sint) DATA_TYPE(R8G8_Srgb)
            DATA_TYPE(R8G8B8_Unorm) DATA_TYPE(R8G8B8_Snorm)
            DATA_TYPE(R8G8B8_Uscaled) DATA_TYPE(R8G8B8_Sscaled)
            DATA_TYPE(R8G8B8_Uint) DATA_TYPE(R8G8B8_Sint)

            DATA_TYPE(R8G8B8_Srgb) DATA_TYPE(B8G8R8_Unorm) DATA_TYPE(B8G8R8_Snorm)
            DATA_TYPE(B8G8R8_Uscaled) DATA_TYPE(B8G8R8_Sscaled)
            DATA_TYPE(B8G8R8_Uint) DATA_TYPE(B8G8R8_Sint) DATA_TYPE(B8G8R8_Srgb)
            DATA_TYPE(R8G8B8A8_Unorm) DATA_TYPE(R8G8B8A8_Snorm)

            DATA_TYPE(R8G8B8A8_Uscaled) DATA_TYPE(R8G8B8A8_Sscaled)
            DATA_TYPE(R8G8B8A8_Uint) DATA_TYPE(R8G8B8A8_Sint) DATA_TYPE(R8G8B8A8_Srgb)
            DATA_TYPE(B8G8R8A8_Unorm) DATA_TYPE(B8G8R8A8_Snorm)
            DATA_TYPE(B8G8R8A8_Uscaled) DATA_TYPE(B8G8R8A8_Sscaled)
            DATA_TYPE(B8G8R8A8_Uint) DATA_TYPE(B8G8R8A8_Sint) DATA_TYPE(B8G8R8A8_Srgb)

            DATA_TYPE(A8B8G8R8_Unorm_Pack32) DATA_TYPE(A8B8G8R8_Snorm_Pack32)
            DATA_TYPE(A8B8G8R8_Uscaled_Pack32) DATA_TYPE(A8B8G8R8_Sscaled_Pack32)
            DATA_TYPE(A8B8G8R8_Uint_Pack32) DATA_TYPE(A8B8G8R8_Sint_Pack32) DATA_TYPE(A8B8G8R8_Srgb_Pack32)
            DATA_TYPE(A2R10G10B10_Unorm_Pack32) DATA_TYPE(A2R10G10B10_Snorm_Pack32) DATA_TYPE(A2R10G10B10_Uscaled_Pack32)

            DATA_TYPE(A2R10G10B10_Sscaled_Pack32) DATA_TYPE(A2R10G10B10_Uint_Pack32) DATA_TYPE(A2R10G10B10_Sint_Pack32)
            DATA_TYPE(A2B10G10R10_Unorm_Pack32) DATA_TYPE(A2B10G10R10_Snorm_Pack32)
            DATA_TYPE(A2B10G10R10_Uscaled_Pack32) DATA_TYPE(A2B10G10R10_Sscaled_Pack32)
            DATA_TYPE(A2B10G10R10_Uint_Pack32) DATA_TYPE(A2B10G10R10_Sint_Pack32)

            DATA_TYPE(R16_Unorm) DATA_TYPE(R16_Snorm)
            DATA_TYPE(R16_Uscaled) DATA_TYPE(R16_Sscaled)
            DATA_TYPE(R16_Uint) DATA_TYPE(R16_Sint) DATA_TYPE(R16_Sfloat)
            DATA_TYPE(R16G16_Unorm) DATA_TYPE(R16G16_Snorm)
            DATA_TYPE(R16G16_Uscaled) DATA_TYPE(R16G16_Sscaled)

            DATA_TYPE(R16G16_Uint) DATA_TYPE(R16G16_Sint) DATA_TYPE(R16G16_Sfloat)
            DATA_TYPE(R16G16B16_Unorm) DATA_TYPE(R16G16B16_Snorm)
            DATA_TYPE(R16G16B16_Uscaled) DATA_TYPE(R16G16B16_Sscaled)
            DATA_TYPE(R16G16B16_Uint) DATA_TYPE(R16G16B16_Sint) DATA_TYPE(R16G16B16_Sfloat)

            DATA_TYPE(R16G16B16A16_Unorm) DATA_TYPE(R16G16B16A16_Snorm)
            DATA_TYPE(R16G16B16A16_Uscaled) DATA_TYPE(R16G16B16A16_Sscaled)
            DATA_TYPE(R16G16B16A16_Uint) DATA_TYPE(R16G16B16A16_Sint) DATA_TYPE(R16G16B16A16_Sfloat)

            DATA_TYPE(R32_Uint) DATA_TYPE(R32_Sint) DATA_TYPE(R32_Sfloat)
            DATA_TYPE(R32G32_Uint) DATA_TYPE(R32G32_Sint) DATA_TYPE(R32G32_Sfloat)
            DATA_TYPE(R32G32B32_Uint) DATA_TYPE(R32G32B32_Sint) DATA_TYPE(R32G32B32_Sfloat)
            DATA_TYPE(R32G32B32A32_Uint) DATA_TYPE(R32G32B32A32_Sint) DATA_TYPE(R32G32B32A32_Sfloat)

            DATA_TYPE(R64_Uint) DATA_TYPE(R64_Sint) DATA_TYPE(R64_Sfloat)
            DATA_TYPE(R64G64_Uint) DATA_TYPE(R64G64_Sint) DATA_TYPE(R64G64_Sfloat)
            DATA_TYPE(R64G64B64_Uint) DATA_TYPE(R64G64B64_Sint) DATA_TYPE(R64G64B64_Sfloat)
            DATA_TYPE(R64G64B64A64_Uint) DATA_TYPE(R64G64B64A64_Sint) DATA_TYPE(R64G64B64A64_Sfloat)

            DATA_TYPE(B10G11R11_Ufloat_Pack32) DATA_TYPE(E5B9G9R9_Ufloat_Pack32)
            DATA_TYPE(D16_Unorm) DATA_TYPE(X8_D24_Unorm_Pack32)
            DATA_TYPE(D32_Sfloat) DATA_TYPE(S8_Uint)
            DATA_TYPE(D16_Unorm_S8_Uint) DATA_TYPE(D24_Unorm_S8_Uint) DATA_TYPE(D32_Sfloat_S8_Uint)
            DATA_TYPE(BC1_RGB_Unorm_Block) DATA_TYPE(BC1_RGB_Srgb_Block)
            DATA_TYPE(BC1_RGBA_Unorm_Block) DATA_TYPE(BC1_RGBA_Srgb_Block) DATA_TYPE(BC2_Unorm_Block)

            DATA_TYPE(BC2_Srgb_Block) DATA_TYPE(BC3_Unorm_Block) DATA_TYPE(BC3_Srgb_Block)
            DATA_TYPE(BC4_Unorm_Block) DATA_TYPE(BC4_Snorm_Block) DATA_TYPE(BC5_Unorm_Block)
            DATA_TYPE(BC5_Snorm_Block) DATA_TYPE(BC6H_Ufloat_Block) DATA_TYPE(BC6H_Sfloat_Block)
            DATA_TYPE(BC7_Unorm_Block) DATA_TYPE(BC7_Srgb_Block)

            DATA_TYPE(ETC2_R8G8B8_Unorm_Block) DATA_TYPE(ETC2_R8G8B8_Srgb_Block)
            DATA_TYPE(ETC2_R8G8B8A1_Unorm_Block) DATA_TYPE(ETC2_R8G8B8A1_Srgb_Block)
            DATA_TYPE(ETC2_R8G8B8A8_Unorm_Block) DATA_TYPE(ETC2_R8G8B8A8_Srgb_Block)
            DATA_TYPE(EAC_R11_Unorm_Block) DATA_TYPE(EAC_R11_Snorm_Block)
            DATA_TYPE(EAC_R11G11_Unorm_Block) DATA_TYPE(EAC_R11G11_Snorm_Block)

            DATA_TYPE(ASTC_4x4_Unorm_Block) DATA_TYPE(ASTC_4x4_Srgb_Block)
            DATA_TYPE(ASTC_5x4_Unorm_Block) DATA_TYPE(ASTC_5x4_Srgb_Block)
            DATA_TYPE(ASTC_5x5_Unorm_Block) DATA_TYPE(ASTC_5x5_Srgb_Block)
            DATA_TYPE(ASTC_6x5_Unorm_Block) DATA_TYPE(ASTC_6x5_Srgb_Block)
            DATA_TYPE(ASTC_6x6_Unorm_Block) DATA_TYPE(ASTC_6x6_Srgb_Block)
            DATA_TYPE(ASTC_8x5_Unorm_Block) DATA_TYPE(ASTC_8x5_Srgb_Block)
            DATA_TYPE(ASTC_8x6_Unorm_Block) DATA_TYPE(ASTC_8x6_Srgb_Block)
            DATA_TYPE(ASTC_8x8_Unorm_Block) DATA_TYPE(ASTC_8x8_Srgb_Block)
            DATA_TYPE(ASTC_10x5_Unorm_Block) DATA_TYPE(ASTC_10x5_Srgb_Block)
            DATA_TYPE(ASTC_10x6_Unorm_Block) DATA_TYPE(ASTC_10x6_Srgb_Block)
            DATA_TYPE(ASTC_10x8_Unorm_Block) DATA_TYPE(ASTC_10x8_Srgb_Block)
            DATA_TYPE(ASTC_10x10_Unorm_Block) DATA_TYPE(ASTC_10x10_Srgb_Block)
            DATA_TYPE(ASTC_12x10_Unorm_Block) DATA_TYPE(ASTC_12x10_Srgb_Block)
            DATA_TYPE(ASTC_12x12_Unorm_Block) DATA_TYPE(ASTC_12x12_Srgb_Block)
            DEFAULT_TYPE(R8G8B8A8_Unorm, TextureFormat)
        }
#undef X
    }

    constexpr TextureTiling STRING_TO_TEXTURE_TILING(const std::string_view str)
    {
#define X TextureTiling
        switch (fnv1a(str))
        {
            DATA_TYPE(Optimal) DATA_TYPE(Linear)
            DEFAULT_TYPE(Optimal, TextureTiling)
        }
#undef X
    }

    constexpr TextureUsageFlags STRING_TO_TEXTURE_USAGE_FLAGS(const std::string_view str)
    {
#define X TextureUsageFlags
        switch (fnv1a(str))
        {
            DATA_TYPE(None) DATA_TYPE(TransferSrc) DATA_TYPE(TransferDst)
            DATA_TYPE(Sampled) DATA_TYPE(Storage) DATA_TYPE(Color)
            DATA_TYPE(DepthStencil) DATA_TYPE(Transient) DATA_TYPE(Input) DATA_TYPE(Host)
            DEFAULT_TYPE(Color, TextureUsageFlags)
        }
#undef X
    }

    constexpr ShaderUsageStageFlags STRING_TO_SHADER_USAGE_FLAGS(const std::string_view str)
    {
#define X ShaderUsageStageFlags
        switch (fnv1a(str))
        {
            DATA_TYPE(None) DATA_TYPE(Vertex)
            DATA_TYPE(TessellationControl) DATA_TYPE(TessellationEvaluation)
            DATA_TYPE(Geometry) DATA_TYPE(Fragment) DATA_TYPE(Compute)
            DATA_TYPE(Default) DATA_TYPE(AllGraphics) DATA_TYPE(All)
            DEFAULT_TYPE(None, ShaderUsageStageFlags)
        }
#undef X
    }

    constexpr TextureResultLayoutType STRING_TO_TEXTURE_RESULT_LAYOUT_TYPE(const std::string_view str)
    {
#define X TextureResultLayoutType
        switch (fnv1a(str))
        {
            DATA_TYPE(Undefined) DATA_TYPE(General) DATA_TYPE(Color)
            DATA_TYPE(DepthStencil) DATA_TYPE(DepthStencilReadOnly) DATA_TYPE(ShaderReadOnly)
            DATA_TYPE(TransferSrc) DATA_TYPE(TransferDst) DATA_TYPE(Preinitialized)
            DATA_TYPE(DepthReadOnly_Stencil) DATA_TYPE(Depth_StencilReadOnly) DATA_TYPE(Depth)
            DATA_TYPE(DepthReadOnly) DATA_TYPE(Stencil) DATA_TYPE(StencilReadOnly)
            DATA_TYPE(ReadOnly) DATA_TYPE(Attachment) DATA_TYPE(RenderingLocalRead)
            DEFAULT_TYPE(ShaderReadOnly, TextureResultLayoutType)
        }
#undef X
    }

    constexpr MipMapMode STRING_TO_MIP_MAP_MODE(const std::string_view str)
    {
#define X MipMapMode
        switch (fnv1a(str))
        {
            DATA_TYPE(Nearest) DATA_TYPE(Linear)
            DEFAULT_TYPE(Nearest, MipMapMode)
        }
#undef X
    }

    constexpr TextureFilter STRING_TO_TEXTURE_FILTER(const std::string_view str)
    {
#define X TextureFilter
        switch (fnv1a(str))
        {
            DATA_TYPE(Point) DATA_TYPE(Bilinear)
            DEFAULT_TYPE(Bilinear, TextureFilter)
        }
#undef X
    }

    constexpr TextureRepeatMode STRING_TO_TEXTURE_REPEAT_MODE(const std::string_view str)
    {
#define X TextureRepeatMode
        switch (fnv1a(str))
        {
            DATA_TYPE(Repeat) DATA_TYPE(MirroredRepeat) DATA_TYPE(ClampToEdge)
            DATA_TYPE(ClampToBorder) DATA_TYPE(MirrorClampToEdge)
            DEFAULT_TYPE(Repeat, TextureRepeatMode)
        }
#undef X
    }

    constexpr ComparisonOperation STRING_TO_COMPARISON_OPERATION(const std::string_view str)
    {
#define X ComparisonOperation
        switch (fnv1a(str))
        {
            DATA_TYPE(None) DATA_TYPE(Never) DATA_TYPE(Less)
            DATA_TYPE(Equal) DATA_TYPE(LessOrEqual)
            DATA_TYPE(Greater) DATA_TYPE(NotEqual)
            DATA_TYPE(GreaterOrEqual) DATA_TYPE(Always)
            DEFAULT_TYPE(LessOrEqual, ComparisonOperation)
        }
#undef X
    }

    constexpr DepthComparisonFunction STRING_TO_DEPTH_COMPARISON_FUNCTION(const std::string_view str)
    {
#define X DepthComparisonFunction
        switch (fnv1a(str))
        {
            DATA_TYPE(Never) DATA_TYPE(Less) DATA_TYPE(Equal)
            DATA_TYPE(LessOrEqual) DATA_TYPE(Greater) DATA_TYPE(NotEqual)
            DATA_TYPE(GreaterOrEqual) DATA_TYPE(Always)
            DEFAULT_TYPE(LessOrEqual, DepthComparisonFunction)
        }
#undef X
    }

    constexpr StencilOperation STRING_TO_STENCIL_OPERATION(const std::string_view str)
    {
#define X StencilOperation
        switch (fnv1a(str))
        {
            DATA_TYPE(Keep) DATA_TYPE(Zero) DATA_TYPE(Replace)
            DATA_TYPE(IncrementAndClamp) DATA_TYPE(DecrementAndClamp) DATA_TYPE(Invert)
            DATA_TYPE(IncrementAndWrap) DATA_TYPE(DecrementAndWrap)
            DEFAULT_TYPE(Keep, StencilOperation)
        }
#undef X
    }

    constexpr MeshPrimitiveTopologyType STRING_TO_PRIMITIVE_TOPOLOGY_TYPE(const std::string_view str)
    {
#define X MeshPrimitiveTopologyType
        switch (fnv1a(str))
        {
            DATA_TYPE(PointList) DATA_TYPE(LineList) DATA_TYPE(LineStrip)
            DATA_TYPE(TriangleList) DATA_TYPE(TriangleStrip) DATA_TYPE(TriangleFan)
            DATA_TYPE(LineListWithAdjacency) DATA_TYPE(LineStripWithAdjacency)
            DATA_TYPE(TriangleListWithAdjacency) DATA_TYPE(TriangleStripWithAdjacency)
            DATA_TYPE(PatchList)
            DEFAULT_TYPE(TriangleList, MeshPrimitiveTopologyType)
        }
#undef X
    }

    constexpr ShaderCullMode STRING_TO_SHADER_CULL_MODE(const std::string_view str)
    {
#define X ShaderCullMode
        switch (fnv1a(str))
        {
            DATA_TYPE(None) DATA_TYPE(Front) DATA_TYPE(Back) DATA_TYPE(Both)
            DEFAULT_TYPE(Back, ShaderCullMode)
        }
#undef X
    }

    constexpr ShaderFaceDirection STRING_TO_SHADER_FACE_DIRECTION(const std::string_view str)
    {
#define X ShaderFaceDirection
        switch (fnv1a(str))
        {
            DATA_TYPE(CounterClockwise) DATA_TYPE(Clockwise)
            DEFAULT_TYPE(CounterClockwise, ShaderFaceDirection)
        }
#undef X
    }

    constexpr MeshPolygonMode STRING_TO_MESH_POLYGON_MODE(const std::string_view str)
    {
#define X MeshPolygonMode
        switch (fnv1a(str))
        {
            DATA_TYPE(Fill) DATA_TYPE(Line) DATA_TYPE(Point) DATA_TYPE(FillRectangleNV)
            DEFAULT_TYPE(Fill, MeshPolygonMode)
        }
#undef X
    }

    constexpr ColorMaskingFlags STRING_TO_COLOR_MASKING_FLAGS(const std::string_view str)
    {
#define X ColorMaskingFlags
        switch (fnv1a(str))
        {
            DATA_TYPE(Red) DATA_TYPE(Green) DATA_TYPE(Blue) DATA_TYPE(Alpha) DATA_TYPE(All)
            DEFAULT_TYPE(All, ColorMaskingFlags)
        }
#undef X
    }

    constexpr ShaderBlendOp STRING_TO_SHADER_BLEND_OPERATION(const std::string_view str)
    {
#define X ShaderBlendOp
        switch (fnv1a(str))
        {
            DATA_TYPE(Clear) DATA_TYPE(And) DATA_TYPE(AndReverse)
            DATA_TYPE(Copy) DATA_TYPE(AndInverted) DATA_TYPE(NoOp)
            DATA_TYPE(XOr) DATA_TYPE(Or) DATA_TYPE(Nor)
            DATA_TYPE(Equivalent) DATA_TYPE(Invert) DATA_TYPE(OrReverse)
            DATA_TYPE(CopyInverted) DATA_TYPE(OrInverted)
            DATA_TYPE(Nand) DATA_TYPE(Set)
            DEFAULT_TYPE(Clear, ShaderBlendOp)
        }
#undef X
    }

    constexpr ColorBlendFactorType STRING_TO_COLOR_BLEND_FACTOR_TYPE(const std::string_view str)
    {
#define X ColorBlendFactorType
        switch (fnv1a(str))
        {
            DATA_TYPE(Zero) DATA_TYPE(One)
            DATA_TYPE(SrcColor) DATA_TYPE(OneMinusSrcColor)
            DATA_TYPE(DstColor) DATA_TYPE(OneMinusDstColor)
            DATA_TYPE(SrcAlpha) DATA_TYPE(OneMinusSrcAlpha)
            DATA_TYPE(DstAlpha) DATA_TYPE(OneMinusDstAlpha)
            DATA_TYPE(ConstantColor) DATA_TYPE(OneMinusConstantColor) DATA_TYPE(ConstantAlpha)
            DATA_TYPE(OneMinusConstantAlpha) DATA_TYPE(SrcAlphaSaturate) DATA_TYPE(Src1Color)
            DATA_TYPE(OneMinusSrc1Color) DATA_TYPE(Src1Alpha) DATA_TYPE(OneMinusSrc1Alpha)
            DEFAULT_TYPE(Zero, ColorBlendFactorType)
        }
#undef X
    }

    constexpr ColorBlendOp STRING_TO_COLOR_BLEND_OPERATION(const std::string_view str)
    {
#define X ColorBlendOp
        switch (fnv1a(str))
        {
            DATA_TYPE(Add) DATA_TYPE(Subtract) DATA_TYPE(ReverseSubtract)
            DATA_TYPE(Min) DATA_TYPE(Max) DATA_TYPE(Zero)
            DATA_TYPE(Src) DATA_TYPE(Dst)
            DATA_TYPE(SrcOver) DATA_TYPE(DstOver)
            DATA_TYPE(SrcIn) DATA_TYPE(DstIn)
            DATA_TYPE(SrcOut) DATA_TYPE(DstOut)
            DATA_TYPE(SrcAtop) DATA_TYPE(DstAtop)
            DATA_TYPE(Xor) DATA_TYPE(Multiply) DATA_TYPE(Screen)
            DATA_TYPE(Overlay) DATA_TYPE(Darken) DATA_TYPE(Lighten)
            DATA_TYPE(ColorDodge) DATA_TYPE(ColorBurn)
            DATA_TYPE(HardLight) DATA_TYPE(SoftLight)
            DATA_TYPE(Difference) DATA_TYPE(Exclusion)
            DATA_TYPE(Invert) DATA_TYPE(InvertRGB)
            DATA_TYPE(LinearDodge) DATA_TYPE(LinearBurn)
            DATA_TYPE(VividLight) DATA_TYPE(LinearLight) DATA_TYPE(PinLight)
            DATA_TYPE(HardMix) DATA_TYPE(HslHue)
            DATA_TYPE(HslSaturation) DATA_TYPE(HslColor) DATA_TYPE(HslLuminosity)
            DATA_TYPE(Plus) DATA_TYPE(PlusClamped)
            DATA_TYPE(PlusClampedAlpha) DATA_TYPE(PlusDarken)
            DATA_TYPE(Minus) DATA_TYPE(MinusClamped)
            DATA_TYPE(Contrast) DATA_TYPE(InvertOVG)
            DATA_TYPE(Red) DATA_TYPE(Green) DATA_TYPE(Blue)
            DEFAULT_TYPE(Add, ColorBlendOp)
        }
#undef X
    }

    constexpr ModelType STRING_TO_MODEL_TYPE(const std::string_view str)
    {
#define X ModelType
        switch (fnv1a(str))
        {
            DATA_TYPE(Unknown) DATA_TYPE(Obj) DATA_TYPE(Fbx) DATA_TYPE(Gltf) DATA_TYPE(Glb)
            DEFAULT_TYPE(Unknown, ModelType)
        }
#undef X
    }

    constexpr BufferInputRate STRING_TO_BUFFER_INPUT_RATE(const std::string_view str)
    {
#define X BufferInputRate
        switch (fnv1a(str))
        {
            DATA_TYPE(Vertex) DATA_TYPE(Instance)
            DEFAULT_TYPE(Vertex, BufferInputRate)
        }
#undef X
    }

    constexpr AudioClipFormat STRING_TO_AUDIO_CLIP_FORMAT(const std::string_view str)
    {
#define X AudioClipFormat
        switch (fnv1a(str))
        {
            DATA_TYPE(Default) DATA_TYPE(U8) DATA_TYPE(S16)
            DATA_TYPE(S24) DATA_TYPE(S32) DATA_TYPE(F32)
            DEFAULT_TYPE(Default, AudioClipFormat)
        }
#undef X
    }

    constexpr AudioClipChannelMixMode STRING_TO_AUDIO_CLIP_CHANNEL_MIX_MODE(const std::string_view str)
    {
#define X AudioClipChannelMixMode
        switch (fnv1a(str))
        {
            DATA_TYPE(Rectangular) DATA_TYPE(Simple) DATA_TYPE(CustomWeights)
            DEFAULT_TYPE(Rectangular, AudioClipChannelMixMode)
        }
#undef X
    }

    constexpr AudioClipDitherMode STRING_TO_AUDIO_CLIP_DITHER_MODE(const std::string_view str)
    {
#define X AudioClipDitherMode
        switch (fnv1a(str))
        {
            DATA_TYPE(None) DATA_TYPE(Rectangle) DATA_TYPE(Triangle)
            DEFAULT_TYPE(None, AudioClipDitherMode)
        }
#undef X
    }

    inline void START_READING(const char* assetPath, List<char>& metadata)
    {
        hassert(utils::FileExists(assetPath), "[Hyperflow] Unable to find meta file: %s", assetPath)
        hassert(utils::ReadFile(assetPath, metadata), "[Hyperflow] Unable to read meta: %s", assetPath)
        metadata.push_back('\0');
    }

    inline void START_READING(const std::filesystem::path& assetPath, List<char>& metadata)
    {
        hassert(utils::FileExists(assetPath), "[Hyperflow] Unable to find meta file: %s", assetPath.string().c_str())
        hassert(utils::ReadFile(assetPath, metadata), "[Hyperflow] Unable to read meta: %s", assetPath.string().c_str())
        metadata.push_back('\0');
    }

#undef DATA_TYPE
}

#endif //HSTRCONVERSION_H
