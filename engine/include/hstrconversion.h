#ifndef HSTRCONVERSION_H
#define HSTRCONVERSION_H

#include "hshared.h"

namespace hf
{
    constexpr uint32_t fnv1a(const std::string_view str)
    {
        uint32_t hash = 2166136261u;
        for (const char c : str)
        {
            hash ^= (uint8_t)c;
            hash *= 16777619u;
        }
        return hash;
    }

    constexpr BufferDataType STRING_TO_BUFFER_DATA_TYPE(const std::string_view str)
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
        default: throw std::invalid_argument("Unknown BufferDataType string");
        }
    }

    constexpr BufferMemoryType STRING_TO_BUFFER_MEMORY_TYPE(const std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("Static"):    return BufferMemoryType::Static;
        case fnv1a("WriteOnly"): return BufferMemoryType::WriteOnly;
        case fnv1a("ReadWrite"): return BufferMemoryType::ReadWrite;
        default: throw std::invalid_argument("Unknown BufferMemoryType string");
        }
    }

    constexpr MeshDataType STRING_TO_MESH_DATA_TYPE(const std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("Position"): return MeshDataType::Position;
        case fnv1a("Normal"):   return MeshDataType::Normal;
        case fnv1a("Color"):    return MeshDataType::Color;
        case fnv1a("TexCoord"): return MeshDataType::TexCoord;
        default: throw std::invalid_argument("Unknown MeshDataType string");
        }
    }

    constexpr TextureChannel STRING_TO_TEXTURE_CHANNEL(const std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("Default"):  return TextureChannel::Default;
        case fnv1a("Gray"):  return TextureChannel::Gray;
        case fnv1a("GrayAlpha"): return TextureChannel::GrayAlpha;
        case fnv1a("RGB"): return TextureChannel::RGB;
        case fnv1a("RGBA"): return TextureChannel::RGBA;
        default: throw std::invalid_argument("Unknown TextureChannel string");
        }
    }

    constexpr TextureAspectFlags STRING_TO_TEXTURE_ASPECT_FLAGS(const std::string_view str)
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
        default: throw std::invalid_argument("Unknown TextureAspectFlags string");
        }
    }

    constexpr TextureFormat STRING_TO_TEXTURE_FORMAT(const std::string_view str)
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

        default: throw std::invalid_argument("Unknown TextureFormat string");
        }
    }

    constexpr TextureTiling STRING_TO_TEXTURE_TILING(const std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("Optimal"): return TextureTiling::Optimal;
        case fnv1a("Linear"):  return TextureTiling::Linear;
        default: throw std::invalid_argument("Unknown TextureChannel string");
        }
    }

    constexpr TextureUsageFlags STRING_TO_TEXTURE_USAGE_FLAGS(const std::string_view str)
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
        default: throw std::invalid_argument("Unknown TextureUsageFlags string");
        }
    }

    constexpr ShaderUsageStageFlags STRING_TO_SHADER_USAGE_FLAGS(const std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("None"):                   return ShaderUsageStageFlags::None;
        case fnv1a("Vertex"):                 return ShaderUsageStageFlags::Vertex;
        case fnv1a("TessellationControl"):    return ShaderUsageStageFlags::TessellationControl;
        case fnv1a("TessellationEvaluation"): return ShaderUsageStageFlags::TessellationEvaluation;
        case fnv1a("Geometry"):               return ShaderUsageStageFlags::Geometry;
        case fnv1a("Fragment"):               return ShaderUsageStageFlags::Fragment;
        case fnv1a("Compute"):                return ShaderUsageStageFlags::Compute;
        case fnv1a("Default"):                return ShaderUsageStageFlags::Default;
        case fnv1a("AllGraphics"):            return ShaderUsageStageFlags::AllGraphics;
        case fnv1a("All"):                    return ShaderUsageStageFlags::All;
        default: throw std::invalid_argument("Unknown TextureUsageFlags string");
        }
    }

    constexpr TextureResultLayoutType STRING_TO_TEXTURE_RESULT_LAYOUT_TYPE(const std::string_view str)
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
        default: throw std::invalid_argument("Unknown TextureUsageFlags string");
        }
    }

    constexpr MipMapMode STRING_TO_MIP_MAP_MODE(const std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("Nearest"): return MipMapMode::Nearest;
        case fnv1a("Linear"):  return MipMapMode::Linear;
        default: throw std::invalid_argument("Unknown MipMap mode string");
        }
    }

    constexpr TextureFilter STRING_TO_TEXTURE_FILTER(const std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("Point"):     return TextureFilter::Point;
        case fnv1a("Bilinear"):  return TextureFilter::Bilinear;
        default: throw std::invalid_argument("Unknown MipMap mode string");
        }
    }

    constexpr TextureRepeatMode STRING_TO_TEXTURE_REPEAT_MODE(const std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("Repeat"):            return TextureRepeatMode::Repeat;
        case fnv1a("MirroredRepeat"):    return TextureRepeatMode::MirroredRepeat;
        case fnv1a("ClampToEdge"):       return TextureRepeatMode::ClampToEdge;
        case fnv1a("ClampToBorder"):     return TextureRepeatMode::ClampToBorder;
        case fnv1a("MirrorClampToEdge"): return TextureRepeatMode::MirrorClampToEdge;
        default: throw std::invalid_argument("Unknown Texture Repeat Mode string");
        }
    }

    constexpr ComparisonOperation STRING_TO_COMPARISON_OPERATION(const std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("None"):           return ComparisonOperation::None;
        case fnv1a("Never"):          return ComparisonOperation::Never;
        case fnv1a("Less"):           return ComparisonOperation::Less;
        case fnv1a("Equal"):          return ComparisonOperation::Equal;
        case fnv1a("LessOrEqual"):    return ComparisonOperation::LessOrEqual;
        case fnv1a("Greater"):        return ComparisonOperation::Greater;
        case fnv1a("NotEqual"):       return ComparisonOperation::NotEqual;
        case fnv1a("GreaterOrEqual"): return ComparisonOperation::GreaterOrEqual;
        case fnv1a("Always"):         return ComparisonOperation::Always;
        default: throw std::invalid_argument("Unknown Comparison Operation string");
        }
    }

    constexpr DepthComparisonFunction STRING_TO_DEPTH_COMPARISON_FUNCTION(const std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("Never"):          return DepthComparisonFunction::Never;
        case fnv1a("Less"):           return DepthComparisonFunction::Less;
        case fnv1a("Equal"):          return DepthComparisonFunction::Equal;
        case fnv1a("LessOrEqual"):    return DepthComparisonFunction::LessOrEqual;
        case fnv1a("Greater"):        return DepthComparisonFunction::Greater;
        case fnv1a("NotEqual"):       return DepthComparisonFunction::NotEqual;
        case fnv1a("GreaterOrEqual"): return DepthComparisonFunction::GreaterOrEqual;
        case fnv1a("Always"):         return DepthComparisonFunction::Always;
        default: throw std::invalid_argument("Unknown Depth Comparison Function string");
        }
    }

    constexpr StencilOperation STRING_TO_STENCIL_OPERATION(const std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("Keep"):              return StencilOperation::Keep;
        case fnv1a("Zero"):              return StencilOperation::Zero;
        case fnv1a("Replace"):           return StencilOperation::Replace;
        case fnv1a("IncrementAndClamp"): return StencilOperation::IncrementAndClamp;
        case fnv1a("DecrementAndClamp"): return StencilOperation::DecrementAndClamp;
        case fnv1a("Invert"):            return StencilOperation::Invert;
        case fnv1a("IncrementAndWrap"):  return StencilOperation::IncrementAndWrap;
        case fnv1a("DecrementAndWrap"):  return StencilOperation::DecrementAndWrap;
        default: throw std::invalid_argument("Unknown Stencil Operation string");
        }
    }

    constexpr MeshPrimitiveTopologyType STRING_TO_PRIMITIVE_TOPOLOGY_TYPE(const std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("PointList"):                  return MeshPrimitiveTopologyType::PointList;
        case fnv1a("LineList"):                   return MeshPrimitiveTopologyType::LineList;
        case fnv1a("LineStrip"):                  return MeshPrimitiveTopologyType::LineStrip;
        case fnv1a("TriangleList"):               return MeshPrimitiveTopologyType::TriangleList;
        case fnv1a("TriangleStrip"):              return MeshPrimitiveTopologyType::TriangleStrip;
        case fnv1a("TriangleFan"):                return MeshPrimitiveTopologyType::TriangleFan;
        case fnv1a("LineListWithAdjacency"):      return MeshPrimitiveTopologyType::LineListWithAdjacency;
        case fnv1a("LineStripWithAdjacency"):     return MeshPrimitiveTopologyType::LineStripWithAdjacency;
        case fnv1a("TriangleListWithAdjacency"):  return MeshPrimitiveTopologyType::TriangleListWithAdjacency;
        case fnv1a("TriangleStripWithAdjacency"): return MeshPrimitiveTopologyType::TriangleStripWithAdjacency;
        case fnv1a("PatchList"):                  return MeshPrimitiveTopologyType::PatchList;
        default: throw std::invalid_argument("Unknown Mesh Primitive Topology Type string");
        }
    }

    constexpr ShaderCullMode STRING_TO_SHADER_CULL_MODE(const std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("None"):  return ShaderCullMode::None;
        case fnv1a("Front"): return ShaderCullMode::Front;
        case fnv1a("Back"):  return ShaderCullMode::Back;
        case fnv1a("Both"):  return ShaderCullMode::Both;
        default: throw std::invalid_argument("Unknown Shader Cull Mode string");
        }
    }

    constexpr ShaderFaceDirection STRING_TO_SHADER_FACE_DIRECTION(const std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("CounterClockwise"): return ShaderFaceDirection::CounterClockwise;
        case fnv1a("Clockwise"):        return ShaderFaceDirection::Clockwise;
        default: throw std::invalid_argument("Unknown Shader Face Direction string");
        }
    }

    constexpr MeshPolygonMode STRING_TO_MESH_POLYGON_MODE(const std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("Fill"):            return MeshPolygonMode::Fill;
        case fnv1a("Line"):            return MeshPolygonMode::Line;
        case fnv1a("Point"):           return MeshPolygonMode::Point;
        case fnv1a("FillRectangleNV"): return MeshPolygonMode::FillRectangleNV;
        default: throw std::invalid_argument("Unknown Mesh Polygon Mode string");
        }
    }

    constexpr ColorMaskingFlags STRING_TO_COLOR_MASKING_FLAGS(const std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("Red"):   return ColorMaskingFlags::Red;
        case fnv1a("Green"): return ColorMaskingFlags::Green;
        case fnv1a("Blue"):  return ColorMaskingFlags::Blue;
        case fnv1a("Alpha"): return ColorMaskingFlags::Alpha;
        case fnv1a("All"):   return ColorMaskingFlags::All;
        default: throw std::invalid_argument("Unknown Color Masking Flags string");
        }
    }

    constexpr ShaderBlendOp STRING_TO_SHADER_BLEND_OPERATION(const std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("Clear"):        return ShaderBlendOp::Clear;
        case fnv1a("And"):          return ShaderBlendOp::And;
        case fnv1a("AndReverse"):   return ShaderBlendOp::AndReverse;
        case fnv1a("Copy"):         return ShaderBlendOp::Copy;
        case fnv1a("AndInverted"):  return ShaderBlendOp::AndInverted;
        case fnv1a("NoOp"):         return ShaderBlendOp::NoOp;
        case fnv1a("XOr"):          return ShaderBlendOp::XOr;
        case fnv1a("Or"):           return ShaderBlendOp::Or;
        case fnv1a("Nor"):          return ShaderBlendOp::Nor;
        case fnv1a("Equivalent"):   return ShaderBlendOp::Equivalent;
        case fnv1a("Invert"):       return ShaderBlendOp::Invert;
        case fnv1a("OrReverse"):    return ShaderBlendOp::OrReverse;
        case fnv1a("CopyInverted"): return ShaderBlendOp::CopyInverted;
        case fnv1a("OrInverted"):   return ShaderBlendOp::OrInverted;
        case fnv1a("Nand"):         return ShaderBlendOp::Nand;
        case fnv1a("Set"):          return ShaderBlendOp::Set;
        default: throw std::invalid_argument("Unknown Shader Blend Operation string");
        }
    }

    constexpr ColorBlendFactorType STRING_TO_COLOR_BLEND_FACTOR_TYPE(const std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("Zero"):                  return ColorBlendFactorType::Zero;
        case fnv1a("One"):                   return ColorBlendFactorType::One;
        case fnv1a("SrcColor"):              return ColorBlendFactorType::SrcColor;
        case fnv1a("OneMinusSrcColor"):      return ColorBlendFactorType::OneMinusSrcColor;
        case fnv1a("DstColor"):              return ColorBlendFactorType::DstColor;
        case fnv1a("OneMinusDstColor"):      return ColorBlendFactorType::OneMinusDstColor;
        case fnv1a("SrcAlpha"):              return ColorBlendFactorType::SrcAlpha;
        case fnv1a("OneMinusSrcAlpha"):      return ColorBlendFactorType::OneMinusSrcAlpha;
        case fnv1a("DstAlpha"):              return ColorBlendFactorType::DstAlpha;
        case fnv1a("OneMinusDstAlpha"):      return ColorBlendFactorType::OneMinusDstAlpha;
        case fnv1a("ConstantColor"):         return ColorBlendFactorType::ConstantColor;
        case fnv1a("OneMinusConstantColor"): return ColorBlendFactorType::OneMinusConstantColor;
        case fnv1a("ConstantAlpha"):         return ColorBlendFactorType::ConstantAlpha;
        case fnv1a("OneMinusConstantAlpha"): return ColorBlendFactorType::OneMinusConstantAlpha;
        case fnv1a("SrcAlphaSaturate"):      return ColorBlendFactorType::SrcAlphaSaturate;
        case fnv1a("Src1Color"):             return ColorBlendFactorType::Src1Color;
        case fnv1a("OneMinusSrc1Color"):     return ColorBlendFactorType::OneMinusSrc1Color;
        case fnv1a("Src1Alpha"):             return ColorBlendFactorType::Src1Alpha;
        case fnv1a("OneMinusSrc1Alpha"):     return ColorBlendFactorType::OneMinusSrc1Alpha;
        default: throw std::invalid_argument("Unknown Color Masking Flags string");
        }
    }

    constexpr ColorBlendOp STRING_TO_COLOR_BLEND_OPERATION(const std::string_view str)
    {
        switch (fnv1a(str))
        {
        case fnv1a("Add"):              return ColorBlendOp::Add;
        case fnv1a("Subtract"):         return ColorBlendOp::Subtract;
        case fnv1a("ReverseSubtract"):  return ColorBlendOp::ReverseSubtract;
        case fnv1a("Min"):              return ColorBlendOp::Min;
        case fnv1a("Max"):              return ColorBlendOp::Max;
        case fnv1a("Zero"):             return ColorBlendOp::Zero;
        case fnv1a("Src"):              return ColorBlendOp::Src;
        case fnv1a("Dst"):              return ColorBlendOp::Dst;
        case fnv1a("SrcOver"):          return ColorBlendOp::SrcOver;
        case fnv1a("DstOver"):          return ColorBlendOp::DstOver;
        case fnv1a("SrcIn"):            return ColorBlendOp::SrcIn;
        case fnv1a("DstIn"):            return ColorBlendOp::DstIn;
        case fnv1a("SrcOut"):           return ColorBlendOp::SrcOut;
        case fnv1a("DstOut"):           return ColorBlendOp::DstOut;
        case fnv1a("SrcAtop"):          return ColorBlendOp::SrcAtop;
        case fnv1a("DstAtop"):          return ColorBlendOp::DstAtop;
        case fnv1a("Xor"):              return ColorBlendOp::Xor;
        case fnv1a("Multiply"):         return ColorBlendOp::Multiply;
        case fnv1a("Screen"):           return ColorBlendOp::Screen;
        case fnv1a("Overlay"):          return ColorBlendOp::Overlay;
        case fnv1a("Darken"):           return ColorBlendOp::Darken;
        case fnv1a("Lighten"):          return ColorBlendOp::Lighten;
        case fnv1a("ColorDodge"):       return ColorBlendOp::ColorDodge;
        case fnv1a("ColorBurn"):        return ColorBlendOp::ColorBurn;
        case fnv1a("HardLight"):        return ColorBlendOp::HardLight;
        case fnv1a("SoftLight"):        return ColorBlendOp::SoftLight;
        case fnv1a("Difference"):       return ColorBlendOp::Difference;
        case fnv1a("Exclusion"):        return ColorBlendOp::Exclusion;
        case fnv1a("Invert"):           return ColorBlendOp::Invert;
        case fnv1a("InvertRGB"):        return ColorBlendOp::InvertRGB;
        case fnv1a("LinearDodge"):      return ColorBlendOp::LinearDodge;
        case fnv1a("LinearBurn"):       return ColorBlendOp::LinearBurn;
        case fnv1a("VividLight"):       return ColorBlendOp::VividLight;
        case fnv1a("LinearLight"):      return ColorBlendOp::LinearLight;
        case fnv1a("PinLight"):         return ColorBlendOp::PinLight;
        case fnv1a("HardMix"):          return ColorBlendOp::HardMix;
        case fnv1a("HslHue"):           return ColorBlendOp::HslHue;
        case fnv1a("HslSaturation"):    return ColorBlendOp::HslSaturation;
        case fnv1a("HslColor"):         return ColorBlendOp::HslColor;
        case fnv1a("HslLuminosity"):    return ColorBlendOp::HslLuminosity;
        case fnv1a("Plus"):             return ColorBlendOp::Plus;
        case fnv1a("PlusClamped"):      return ColorBlendOp::PlusClamped;
        case fnv1a("PlusClampedAlpha"): return ColorBlendOp::PlusClampedAlpha;
        case fnv1a("PlusDarken"):       return ColorBlendOp::PlusDarken;
        case fnv1a("Minus"):            return ColorBlendOp::Minus;
        case fnv1a("MinusClamped"):     return ColorBlendOp::MinusClamped;
        case fnv1a("Contrast"):         return ColorBlendOp::Contrast;
        case fnv1a("InvertOVG"):        return ColorBlendOp::InvertOVG;
        case fnv1a("Red"):              return ColorBlendOp::Red;
        case fnv1a("Green"):            return ColorBlendOp::Green;
        case fnv1a("Blue"):             return ColorBlendOp::Blue;
        default: throw std::invalid_argument("Unknown Color Masking Flags string");
        }
    }

    inline bool START_READING(const char* assetPath, std::vector<char>& metadata)
    {
        if (!utils::FileExists(assetPath))
        {
            LOG_ERROR("[Hyperflow] Unable to find meta file: %s", assetPath);
            return false;
        }

        if (!utils::ReadFile(assetPath, metadata))
        {
            LOG_ERROR("[Hyperflow] Unable to read meta: %s", assetPath);
            return false;
        }
        metadata.push_back('\0');
        return true;
    }
}

#endif //HSTRCONVERSION_H
