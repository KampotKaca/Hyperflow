#ifndef HSTRCONVERSION_H
#define HSTRCONVERSION_H

#include "hshared.h"

namespace hf
{
    constexpr uint32_t fnv1a(std::string_view str)
    {
        uint32_t hash = 2166136261u;
        for (const char c : str)
        {
            hash ^= static_cast<uint8_t>(c);
            hash *= 16777619u;
        }
        return hash;
    }

    constexpr BufferDataType STRING_TO_BUFFER_DATA_TYPE(std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("U8"):  return BufferDataType::U8;
        case fnv1a("I8"):  return BufferDataType::I8;
        case fnv1a("U16"): return BufferDataType::U16;
        case fnv1a("I16"): return BufferDataType::I16;
        case fnv1a("U32"): return BufferDataType::U32;
        case fnv1a("I32"): return BufferDataType::I32;
        case fnv1a("U64"): return BufferDataType::U64;
        case fnv1a("I64"): return BufferDataType::I64;
        case fnv1a("F16"): return BufferDataType::F16;
        case fnv1a("F32"): return BufferDataType::F32;
        case fnv1a("F64"): return BufferDataType::F64;
        default:
            throw std::invalid_argument("Unknown BufferDataType string");
        }
    }

    constexpr BufferMemoryType STRING_TO_BUFFER_MEMORY_TYPE(std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("Static"):    return BufferMemoryType::Static;
        case fnv1a("WriteOnly"): return BufferMemoryType::WriteOnly;
        case fnv1a("ReadWrite"): return BufferMemoryType::ReadWrite;
        default:
            throw std::invalid_argument("Unknown BufferMemoryType string");
        }
    }

    constexpr MeshDataType STRING_TO_MESH_DATA_TYPE(std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("Position"): return MeshDataType::Position;
        case fnv1a("Normal"):   return MeshDataType::Normal;
        case fnv1a("Color"):    return MeshDataType::Color;
        case fnv1a("TexCoord"): return MeshDataType::TexCoord;
        default:
            throw std::invalid_argument("Unknown MeshDataType string");
        }
    }

    constexpr TextureType STRING_TO_TEXTURE_TYPE(std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("Tex1D"): return TextureType::Tex1D;
        case fnv1a("Tex2D"): return TextureType::Tex2D;
        case fnv1a("Tex3D"): return TextureType::Tex3D;
        default:
            throw std::invalid_argument("Unknown TextureChannel string");
        }
    }

    constexpr TextureChannel STRING_TO_TEXTURE_CHANNEL(std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("Default"):  return TextureChannel::Default;
        case fnv1a("Gray"):  return TextureChannel::Gray;
        case fnv1a("GrayAlpha"): return TextureChannel::GrayAlpha;
        case fnv1a("RGB"): return TextureChannel::RGB;
        case fnv1a("RGBA"): return TextureChannel::RGBA;
        default:
            throw std::invalid_argument("Unknown TextureChannel string");
        }
    }

    constexpr TextureAspectFlags STRING_TO_TEXTURE_ASPECT_FLAGS(std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("None"):  return TextureAspectFlags::None;
        case fnv1a("Color"):  return TextureAspectFlags::Color;
        case fnv1a("Depth"): return TextureAspectFlags::Depth;
        case fnv1a("Stencil"): return TextureAspectFlags::Stencil;
        case fnv1a("MetaData"): return TextureAspectFlags::MetaData;
        case fnv1a("Plane0"): return TextureAspectFlags::Plane0;
        case fnv1a("Plane1"): return TextureAspectFlags::Plane1;
        case fnv1a("Plane2"): return TextureAspectFlags::Plane2;
        default:
            throw std::invalid_argument("Unknown TextureAspectFlags string");
        }
    }

    constexpr TextureFormat STRING_TO_TEXTURE_FORMAT(std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("Undefined"):                return TextureFormat::Undefined;                //0
        case fnv1a("R4G4_Unorm_Pack8"):         return TextureFormat::R4G4_Unorm_Pack8;         //1
        case fnv1a("R4G4B4A4_Unorm_Pack16"):    return TextureFormat::R4G4B4A4_Unorm_Pack16;    //2
        case fnv1a("B4G4R4A4_Unorm_Pack16"):    return TextureFormat::B4G4R4A4_Unorm_Pack16;    //3
        case fnv1a("R5G6B5_Unorm_Pack16"):      return TextureFormat::R5G6B5_Unorm_Pack16;      //4
        case fnv1a("B5G6R5_Unorm_Pack16"):      return TextureFormat::B5G6R5_Unorm_Pack16;      //5
        case fnv1a("R5G5B5A1_Unorm_Pack16"):    return TextureFormat::R5G5B5A1_Unorm_Pack16;    //6
        case fnv1a("B5G5R5A1_Unorm_Pack16"):    return TextureFormat::B5G5R5A1_Unorm_Pack16;    //7
        case fnv1a("A1R5G5B5_Unorm_Pack16"):    return TextureFormat::A1R5G5B5_Unorm_Pack16;    //8

        case fnv1a("R8_Unorm"):         return TextureFormat::R8_Unorm;         //9
        case fnv1a("R8_Snorm"):         return TextureFormat::R8_Snorm;         //10
        case fnv1a("R8_Uscaled"):       return TextureFormat::R8_Uscaled;       //11
        case fnv1a("R8_Sscaled"):       return TextureFormat::R8_Sscaled;       //12
        case fnv1a("R8_Uint"):          return TextureFormat::R8_Uint;          //13
        case fnv1a("R8_Sint"):          return TextureFormat::R8_Sint;          //14
        case fnv1a("R8_Srgb"):          return TextureFormat::R8_Srgb;          //15
        case fnv1a("R8G8_Unorm"):       return TextureFormat::R8G8_Unorm;       //16
        case fnv1a("R8G8_Snorm"):       return TextureFormat::R8G8_Snorm;       //17
        case fnv1a("R8G8_Uscaled"):     return TextureFormat::R8G8_Uscaled;     //18
        case fnv1a("R8G8_Sscaled"):     return TextureFormat::R8G8_Sscaled;     //19

        case fnv1a("R8G8_Uint"):        return TextureFormat::R8G8_Uint;        //20
        case fnv1a("R8G8_Sint"):        return TextureFormat::R8G8_Sint;        //21
        case fnv1a("R8G8_Srgb"):        return TextureFormat::R8G8_Srgb;        //22
        case fnv1a("R8G8B8_Unorm"):     return TextureFormat::R8G8B8_Unorm;     //23
        case fnv1a("R8G8B8_Snorm"):     return TextureFormat::R8G8B8_Snorm;     //24
        case fnv1a("R8G8B8_Uscaled"):   return TextureFormat::R8G8B8_Uscaled;   //25
        case fnv1a("R8G8B8_Sscaled"):   return TextureFormat::R8G8B8_Sscaled;   //26
        case fnv1a("R8G8B8_Uint"):      return TextureFormat::R8G8B8_Uint;      //27
        case fnv1a("R8G8B8_Sint"):      return TextureFormat::R8G8B8_Sint;      //28

        case fnv1a("R8G8B8_Srgb"):      return TextureFormat::R8G8B8_Srgb;      //29
        case fnv1a("B8G8R8_Unorm"):     return TextureFormat::B8G8R8_Unorm;     //30
        case fnv1a("B8G8R8_Snorm"):     return TextureFormat::B8G8R8_Snorm;     //31
        case fnv1a("B8G8R8_Uscaled"):   return TextureFormat::B8G8R8_Uscaled;   //32
        case fnv1a("B8G8R8_Sscaled"):   return TextureFormat::B8G8R8_Sscaled;   //33
        case fnv1a("B8G8R8_Uint"):      return TextureFormat::B8G8R8_Uint;      //34
        case fnv1a("B8G8R8_Sint"):      return TextureFormat::B8G8R8_Sint;      //35
        case fnv1a("B8G8R8_Srgb"):      return TextureFormat::B8G8R8_Srgb;      //36
        case fnv1a("R8G8B8A8_Unorm"):   return TextureFormat::R8G8B8A8_Unorm;   //37
        case fnv1a("R8G8B8A8_Snorm"):   return TextureFormat::R8G8B8A8_Snorm;   //38

        case fnv1a("R8G8B8A8_Uscaled"):   return TextureFormat::R8G8B8A8_Uscaled;   //39
        case fnv1a("R8G8B8A8_Sscaled"):   return TextureFormat::R8G8B8A8_Sscaled;   //40
        case fnv1a("R8G8B8A8_Uint"):      return TextureFormat::R8G8B8A8_Uint;      //41
        case fnv1a("R8G8B8A8_Sint"):      return TextureFormat::R8G8B8A8_Sint;      //42
        case fnv1a("R8G8B8A8_Srgb"):      return TextureFormat::R8G8B8A8_Srgb;      //43
        case fnv1a("B8G8R8A8_Unorm"):     return TextureFormat::B8G8R8A8_Unorm;     //44
        case fnv1a("B8G8R8A8_Snorm"):     return TextureFormat::B8G8R8A8_Snorm;     //45
        case fnv1a("B8G8R8A8_Uscaled"):   return TextureFormat::B8G8R8A8_Uscaled;   //46
        case fnv1a("B8G8R8A8_Sscaled"):   return TextureFormat::B8G8R8A8_Sscaled;   //47
        case fnv1a("B8G8R8A8_Uint"):      return TextureFormat::B8G8R8A8_Uint;      //48
        case fnv1a("B8G8R8A8_Sint"):      return TextureFormat::B8G8R8A8_Sint;      //49
        case fnv1a("B8G8R8A8_Srgb"):      return TextureFormat::B8G8R8A8_Srgb;      //50

        case fnv1a("A8B8G8R8_Unorm_Pack32"):      return TextureFormat::A8B8G8R8_Unorm_Pack32;     //51
        case fnv1a("A8B8G8R8_Snorm_Pack32"):      return TextureFormat::A8B8G8R8_Snorm_Pack32;     //52
        case fnv1a("A8B8G8R8_Uscaled_Pack32"):    return TextureFormat::A8B8G8R8_Uscaled_Pack32;   //53
        case fnv1a("A8B8G8R8_Sscaled_Pack32"):    return TextureFormat::A8B8G8R8_Sscaled_Pack32;   //54
        case fnv1a("A8B8G8R8_Uint_Pack32"):       return TextureFormat::A8B8G8R8_Uint_Pack32;      //55
        case fnv1a("A8B8G8R8_Sint_Pack32"):       return TextureFormat::A8B8G8R8_Sint_Pack32;      //56
        case fnv1a("A8B8G8R8_Srgb_Pack32"):       return TextureFormat::A8B8G8R8_Srgb_Pack32;      //57
        case fnv1a("A2R10G10B10_Unorm_Pack32"):   return TextureFormat::A2R10G10B10_Unorm_Pack32;  //58
        case fnv1a("A2R10G10B10_Snorm_Pack32"):   return TextureFormat::A2R10G10B10_Snorm_Pack32;  //59
        case fnv1a("A2R10G10B10_Uscaled_Pack32"): return TextureFormat::A2R10G10B10_Uscaled_Pack32;//60

        case fnv1a("A2R10G10B10_Sscaled_Pack32"): return TextureFormat::A2R10G10B10_Sscaled_Pack32;//61
        case fnv1a("A2R10G10B10_Uint_Pack32"): return TextureFormat::A2R10G10B10_Uint_Pack32;      //62
        case fnv1a("A2R10G10B10_Sint_Pack32"): return TextureFormat::A2R10G10B10_Sint_Pack32;      //63
        case fnv1a("A2B10G10R10_Unorm_Pack32"): return TextureFormat::A2B10G10R10_Unorm_Pack32;    //64
        case fnv1a("A2B10G10R10_Snorm_Pack32"): return TextureFormat::A2B10G10R10_Snorm_Pack32;    //65
        case fnv1a("A2B10G10R10_Uscaled_Pack32"): return TextureFormat::A2B10G10R10_Uscaled_Pack32;//66
        case fnv1a("A2B10G10R10_Sscaled_Pack32"): return TextureFormat::A2B10G10R10_Sscaled_Pack32;//67
        case fnv1a("A2B10G10R10_Uint_Pack32"): return TextureFormat::A2B10G10R10_Uint_Pack32;      //68
        case fnv1a("A2B10G10R10_Sint_Pack32"): return TextureFormat::A2B10G10R10_Sint_Pack32;      //69

        case fnv1a("R16_Unorm"):        return TextureFormat::R16_Unorm;      //70
        case fnv1a("R16_Snorm"):        return TextureFormat::R16_Snorm;      //71
        case fnv1a("R16_Uscaled"):      return TextureFormat::R16_Uscaled;    //72
        case fnv1a("R16_Sscaled"):      return TextureFormat::R16_Sscaled;    //73
        case fnv1a("R16_Uint"):         return TextureFormat::R16_Uint;       //74
        case fnv1a("R16_Sint"):         return TextureFormat::R16_Sint;       //75
        case fnv1a("R16_Sfloat"):       return TextureFormat::R16_Sfloat;     //76
        case fnv1a("R16G16_Unorm"):     return TextureFormat::R16G16_Unorm;   //77
        case fnv1a("R16G16_Snorm"):     return TextureFormat::R16G16_Snorm;   //78
        case fnv1a("R16G16_Uscaled"):   return TextureFormat::R16G16_Uscaled; //79
        case fnv1a("R16G16_Sscaled"):   return TextureFormat::R16G16_Sscaled; //80

        case fnv1a("R16G16_Uint"):      return TextureFormat::R16G16_Uint;        //81
        case fnv1a("R16G16_Sint"):      return TextureFormat::R16G16_Sint;        //82
        case fnv1a("R16G16_Sfloat"):    return TextureFormat::R16G16_Sfloat;      //83
        case fnv1a("R16G16B16_Unorm"):  return TextureFormat::R16G16B16_Unorm;    //84
        case fnv1a("R16G16B16_Snorm"):  return TextureFormat::R16G16B16_Snorm;    //85
        case fnv1a("R16G16B16_Uscaled"):return TextureFormat::R16G16B16_Uscaled;  //86
        case fnv1a("R16G16B16_Sscaled"):return TextureFormat::R16G16B16_Sscaled;  //87
        case fnv1a("R16G16B16_Uint"):   return TextureFormat::R16G16B16_Uint;     //88
        case fnv1a("R16G16B16_Sint"):   return TextureFormat::R16G16B16_Sint;     //89
        case fnv1a("R16G16B16_Sfloat"): return TextureFormat::R16G16B16_Sfloat;   //90

        case fnv1a("R16G16B16A16_Unorm"):   return TextureFormat::R16G16B16A16_Unorm;     //91
        case fnv1a("R16G16B16A16_Snorm"):   return TextureFormat::R16G16B16A16_Snorm;     //92
        case fnv1a("R16G16B16A16_Uscaled"): return TextureFormat::R16G16B16A16_Uscaled;   //93
        case fnv1a("R16G16B16A16_Sscaled"): return TextureFormat::R16G16B16A16_Sscaled;   //94
        case fnv1a("R16G16B16A16_Uint"):    return TextureFormat::R16G16B16A16_Uint;      //95
        case fnv1a("R16G16B16A16_Sint"):    return TextureFormat::R16G16B16A16_Sint;      //96
        case fnv1a("R16G16B16A16_Sfloat"):  return TextureFormat::R16G16B16A16_Sfloat;    //97

        case fnv1a("R32_Uint"):             return TextureFormat::R32_Uint;             //98
        case fnv1a("R32_Sint"):             return TextureFormat::R32_Sint;             //99
        case fnv1a("R32_Sfloat"):           return TextureFormat::R32_Sfloat;           //100
        case fnv1a("R32G32_Uint"):          return TextureFormat::R32G32_Uint;          //101
        case fnv1a("R32G32_Sint"):          return TextureFormat::R32G32_Sint;          //102
        case fnv1a("R32G32_Sfloat"):        return TextureFormat::R32G32_Sfloat;        //103
        case fnv1a("R32G32B32_Uint"):       return TextureFormat::R32G32B32_Uint;       //104
        case fnv1a("R32G32B32_Sint"):       return TextureFormat::R32G32B32_Sint;       //105
        case fnv1a("R32G32B32_Sfloat"):     return TextureFormat::R32G32B32_Sfloat;     //106
        case fnv1a("R32G32B32A32_Uint"):    return TextureFormat::R32G32B32A32_Uint;    //107
        case fnv1a("R32G32B32A32_Sint"):    return TextureFormat::R32G32B32A32_Sint;    //108
        case fnv1a("R32G32B32A32_Sfloat"):  return TextureFormat::R32G32B32A32_Sfloat;  //109

        case fnv1a("R64_Uint"):             return TextureFormat::R64_Uint;             //110
        case fnv1a("R64_Sint"):             return TextureFormat::R64_Sint;             //111
        case fnv1a("R64_Sfloat"):           return TextureFormat::R64_Sfloat;           //112
        case fnv1a("R64G64_Uint"):          return TextureFormat::R64G64_Uint;          //113
        case fnv1a("R64G64_Sint"):          return TextureFormat::R64G64_Sint;          //114
        case fnv1a("R64G64_Sfloat"):        return TextureFormat::R64G64_Sfloat;        //115
        case fnv1a("R64G64B64_Uint"):       return TextureFormat::R64G64B64_Uint;       //116
        case fnv1a("R64G64B64_Sint"):       return TextureFormat::R64G64B64_Sint;       //117
        case fnv1a("R64G64B64_Sfloat"):     return TextureFormat::R64G64B64_Sfloat;     //118
        case fnv1a("R64G64B64A64_Uint"):    return TextureFormat::R64G64B64A64_Uint;    //119
        case fnv1a("R64G64B64A64_Sint"):    return TextureFormat::R64G64B64A64_Sint;    //120

        case fnv1a("R64G64B64A64_Sfloat"):      return TextureFormat::R64G64B64A64_Sfloat;      //121
        case fnv1a("B10G11R11_Ufloat_Pack32"):  return TextureFormat::B10G11R11_Ufloat_Pack32;  //122
        case fnv1a("E5B9G9R9_Ufloat_Pack32"):   return TextureFormat::E5B9G9R9_Ufloat_Pack32;   //123
        case fnv1a("D16_Unorm"):                return TextureFormat::D16_Unorm;                //124
        case fnv1a("X8_D24_Unorm_Pack32"):      return TextureFormat::X8_D24_Unorm_Pack32;      //125
        case fnv1a("D32_Sfloat"):               return TextureFormat::D32_Sfloat;               //126
        case fnv1a("S8_Uint"):                  return TextureFormat::S8_Uint;                  //127
        case fnv1a("D16_Unorm_S8_Uint"):        return TextureFormat::D16_Unorm_S8_Uint;        //128
        case fnv1a("D24_Unorm_S8_Uint"):        return TextureFormat::D24_Unorm_S8_Uint;        //129
        case fnv1a("D32_Sfloat_S8_Uint"):       return TextureFormat::D32_Sfloat_S8_Uint;       //130
        case fnv1a("BC1_RGB_Unorm_Block"):      return TextureFormat::BC1_RGB_Unorm_Block;      //131
        case fnv1a("BC1_RGB_Srgb_Block"):       return TextureFormat::BC1_RGB_Srgb_Block;       //132
        case fnv1a("BC1_RGBA_Unorm_Block"):     return TextureFormat::BC1_RGBA_Unorm_Block;     //133
        case fnv1a("BC1_RGBA_Srgb_Block"):      return TextureFormat::BC1_RGBA_Srgb_Block;      //134
        case fnv1a("BC2_Unorm_Block"):          return TextureFormat::BC2_Unorm_Block;          //135

        case fnv1a("BC2_Srgb_Block"):     return TextureFormat::BC2_Srgb_Block;           //136
        case fnv1a("BC3_Unorm_Block"):    return TextureFormat::BC3_Unorm_Block;          //137
        case fnv1a("BC3_Srgb_Block"):     return TextureFormat::BC3_Srgb_Block;           //138
        case fnv1a("BC4_Unorm_Block"):    return TextureFormat::BC4_Unorm_Block;          //139
        case fnv1a("BC4_Snorm_Block"):    return TextureFormat::BC4_Snorm_Block;          //140
        case fnv1a("BC5_Unorm_Block"):    return TextureFormat::BC5_Unorm_Block;          //141
        case fnv1a("BC5_Snorm_Block"):    return TextureFormat::BC5_Snorm_Block;          //142
        case fnv1a("BC6H_Ufloat_Block"):  return TextureFormat::BC6H_Ufloat_Block;        //143
        case fnv1a("BC6H_Sfloat_Block"):  return TextureFormat::BC6H_Sfloat_Block;        //144
        case fnv1a("BC7_Unorm_Block"):    return TextureFormat::BC7_Unorm_Block;          //145

        case fnv1a("BC7_Srgb_Block"):             return TextureFormat::BC7_Srgb_Block;             //146
        case fnv1a("ETC2_R8G8B8_Unorm_Block"):    return TextureFormat::ETC2_R8G8B8_Unorm_Block;    //147
        case fnv1a("ETC2_R8G8B8_Srgb_Block"):     return TextureFormat::ETC2_R8G8B8_Srgb_Block;     //148
        case fnv1a("ETC2_R8G8B8A1_Unorm_Block"):  return TextureFormat::ETC2_R8G8B8A1_Unorm_Block;  //149
        case fnv1a("ETC2_R8G8B8A1_Srgb_Block"):   return TextureFormat::ETC2_R8G8B8A1_Srgb_Block;   //150
        case fnv1a("ETC2_R8G8B8A8_Unorm_Block"):  return TextureFormat::ETC2_R8G8B8A8_Unorm_Block;  //151
        case fnv1a("ETC2_R8G8B8A8_Srgb_Block"):   return TextureFormat::ETC2_R8G8B8A8_Srgb_Block;   //152
        case fnv1a("EAC_R11_Unorm_Block"):        return TextureFormat::EAC_R11_Unorm_Block;        //153
        case fnv1a("EAC_R11_Snorm_Block"):        return TextureFormat::EAC_R11_Snorm_Block;        //154
        case fnv1a("EAC_R11G11_Unorm_Block"):     return TextureFormat::EAC_R11G11_Unorm_Block;     //155

        case fnv1a("EAC_R11G11_Snorm_Block"):  return TextureFormat::EAC_R11G11_Snorm_Block;  //156
        case fnv1a("ASTC_4x4_Unorm_Block"):    return TextureFormat::ASTC_4x4_Unorm_Block;    //157
        case fnv1a("ASTC_4x4_Srgb_Block"):     return TextureFormat::ASTC_4x4_Srgb_Block;     //158
        case fnv1a("ASTC_5x4_Unorm_Block"):    return TextureFormat::ASTC_5x4_Unorm_Block;    //159
        case fnv1a("ASTC_5x4_Srgb_Block"):     return TextureFormat::ASTC_5x4_Srgb_Block;     //160
        case fnv1a("ASTC_5x5_Unorm_Block"):    return TextureFormat::ASTC_5x5_Unorm_Block;    //161
        case fnv1a("ASTC_5x5_Srgb_Block"):     return TextureFormat::ASTC_5x5_Srgb_Block;     //162
        case fnv1a("ASTC_6x5_Unorm_Block"):    return TextureFormat::ASTC_6x5_Unorm_Block;    //163
        case fnv1a("ASTC_6x5_Srgb_Block"):     return TextureFormat::ASTC_6x5_Srgb_Block;     //164
        case fnv1a("ASTC_6x6_Unorm_Block"):    return TextureFormat::ASTC_6x6_Unorm_Block;    //165

        case fnv1a("ASTC_6x6_Srgb_Block"):    return TextureFormat::ASTC_6x6_Srgb_Block;   //166
        case fnv1a("ASTC_8x5_Unorm_Block"):   return TextureFormat::ASTC_8x5_Unorm_Block;  //167
        case fnv1a("ASTC_8x5_Srgb_Block"):    return TextureFormat::ASTC_8x5_Srgb_Block;   //168
        case fnv1a("ASTC_8x6_Unorm_Block"):   return TextureFormat::ASTC_8x6_Unorm_Block;  //169
        case fnv1a("ASTC_8x6_Srgb_Block"):    return TextureFormat::ASTC_8x6_Srgb_Block;   //170
        case fnv1a("ASTC_8x8_Unorm_Block"):   return TextureFormat::ASTC_8x8_Unorm_Block;  //171
        case fnv1a("ASTC_8x8_Srgb_Block"):    return TextureFormat::ASTC_8x8_Srgb_Block;   //172
        case fnv1a("ASTC_10x5_Unorm_Block"):  return TextureFormat::ASTC_10x5_Unorm_Block; //173
        case fnv1a("ASTC_10x5_Srgb_Block"):   return TextureFormat::ASTC_10x5_Srgb_Block;  //174
        case fnv1a("ASTC_10x6_Unorm_Block"):  return TextureFormat::ASTC_10x6_Unorm_Block; //175

        case fnv1a("ASTC_10x6_Srgb_Block"):   return TextureFormat::ASTC_10x6_Srgb_Block;   //176
        case fnv1a("ASTC_10x8_Unorm_Block"):  return TextureFormat::ASTC_10x8_Unorm_Block;  //177
        case fnv1a("ASTC_10x8_Srgb_Block"):   return TextureFormat::ASTC_10x8_Srgb_Block;   //178
        case fnv1a("ASTC_10x10_Unorm_Block"): return TextureFormat::ASTC_10x10_Unorm_Block; //179
        case fnv1a("ASTC_10x10_Srgb_Block"):  return TextureFormat::ASTC_10x10_Srgb_Block;  //180
        case fnv1a("ASTC_12x10_Unorm_Block"): return TextureFormat::ASTC_12x10_Unorm_Block; //181
        case fnv1a("ASTC_12x10_Srgb_Block"):  return TextureFormat::ASTC_12x10_Srgb_Block;  //182
        case fnv1a("ASTC_12x12_Unorm_Block"): return TextureFormat::ASTC_12x12_Unorm_Block; //183
        case fnv1a("ASTC_12x12_Srgb_Block"):  return TextureFormat::ASTC_12x12_Srgb_Block;  //184

        default:
            throw std::invalid_argument("Unknown TextureFormat string");
        }
    }

    constexpr TextureTiling STRING_TO_TEXTURE_TILING(std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("Optimal"): return TextureTiling::Optimal;
        case fnv1a("Linear"):  return TextureTiling::Linear;
        default:
            throw std::invalid_argument("Unknown TextureChannel string");
        }
    }

    constexpr TextureUsageFlags STRING_TO_TEXTURE_USAGE_FLAGS(std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("None"):         return TextureUsageFlags::None;
        case fnv1a("TransferSrc"):  return TextureUsageFlags::TransferSrc;
        case fnv1a("TransferDst"):  return TextureUsageFlags::TransferDst;
        case fnv1a("Sampled"):      return TextureUsageFlags::Sampled;
        case fnv1a("Storage"):      return TextureUsageFlags::Storage;
        case fnv1a("Color"):        return TextureUsageFlags::Color;
        case fnv1a("DepthStencil"): return TextureUsageFlags::DepthStencil;
        case fnv1a("Transient"):    return TextureUsageFlags::Transient;
        case fnv1a("Input"):        return TextureUsageFlags::Input;
        case fnv1a("Host"):         return TextureUsageFlags::Host;
        default:
            throw std::invalid_argument("Unknown TextureUsageFlags string");
        }
    }

    constexpr TextureResultLayoutType STRING_TO_TEXTURE_RESULT_LAYOUT_TYPE(std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("Undefined"):                return TextureResultLayoutType::Undefined;
        case fnv1a("General"):                  return TextureResultLayoutType::General;
        case fnv1a("Color"):                    return TextureResultLayoutType::Color;
        case fnv1a("DepthStencil"):             return TextureResultLayoutType::DepthStencil;
        case fnv1a("DepthStencilReadOnly"):     return TextureResultLayoutType::DepthStencilReadOnly;
        case fnv1a("ShaderReadOnly"):           return TextureResultLayoutType::ShaderReadOnly;
        case fnv1a("TransferSrc"):              return TextureResultLayoutType::TransferSrc;
        case fnv1a("TransferDst"):              return TextureResultLayoutType::TransferDst;
        case fnv1a("Preinitialized"):           return TextureResultLayoutType::Preinitialized;
        case fnv1a("DepthReadOnly_Stencil"):    return TextureResultLayoutType::DepthReadOnly_Stencil;
        case fnv1a("Depth_StencilReadOnly"):    return TextureResultLayoutType::Depth_StencilReadOnly;
        case fnv1a("Depth"):                    return TextureResultLayoutType::Depth;
        case fnv1a("DepthReadOnly"):            return TextureResultLayoutType::DepthReadOnly;
        case fnv1a("Stencil"):                  return TextureResultLayoutType::Stencil;
        case fnv1a("StencilReadOnly"):          return TextureResultLayoutType::StencilReadOnly;
        case fnv1a("ReadOnly"):                 return TextureResultLayoutType::ReadOnly;
        case fnv1a("Attachment"):               return TextureResultLayoutType::Attachment;
        case fnv1a("RenderingLocalRead"):       return TextureResultLayoutType::RenderingLocalRead;
        default:
            throw std::invalid_argument("Unknown TextureUsageFlags string");
        }
    }
}

#endif //HSTRCONVERSION_H
