#ifndef HUTILS_H
#define HUTILS_H

#include "heditorinternal.h"
#include "imgui_internal.h"
#include "hyperfloweditor.h"

namespace hf::ed
{
    inline bool operator !=(const ImVec2& v1, const ImVec2& v2) { return v1.x != v2.x || v1.y != v2.y; }
    inline bool operator ==(const ImVec2& v1, const ImVec2& v2) { return v1.x == v2.x && v1.y == v2.y; }

    inline ImVec2 operator -(const ImVec2& v1, const ImVec2& v2) { return { v1.x - v2.x, v1.y - v2.y }; }
    inline void operator -=(ImVec2& v1, const ImVec2& v2) { v1.x -= v2.x; v1.y -= v2.y; }

    inline ImVec2 operator +(const ImVec2& v1, const ImVec2& v2) { return { v1.x + v2.x, v1.y + v2.y }; }
    inline void operator +=(ImVec2& v1, const ImVec2& v2) { v1.x += v2.x; v1.y += v2.y; }

    inline ImVec2 operator *(const ImVec2& v1, float f) { return { v1.x * f, v1.y * f }; }
    inline void operator *=(ImVec2& v1, float f) { v1.x *= f; v1.y *= f; }

    static const ImVec4 VECTOR_STYLES[] =
    {
        { 0.8f, 0.1f, 0.15f, 1.0f }, { 0.9f, 0.2f, 0.2f, 1.0f }, { 0.8f, 0.1f, 0.15f, 1.0f },
        { 0.2f, 0.7f, 0.2f, 1.0f  }, { 0.3f, 0.8f, 0.3f, 1.0f }, { 0.2f, 0.7f, 0.2f, 1.0f },
        { 0.1f, 0.25f, 0.8f, 1.0f }, { 0.2f, 0.35f, 0.9f, 1.0f}, { 0.1f, 0.25f, 0.8f, 1.0f },
        { 0.5f, 0.43f, 0.7f, 1.0f }, { 0.6f, 0.53f, 0.8f, 1.0f}, { 0.5f, 0.43f, 0.7f, 1.0f },
    };

    static const ImGuiDataTypeInfo GDataTypeInfo[] =
    {
        { sizeof(char),             "S8",   "%d",   "%d"    },
        { sizeof(unsigned char),    "U8",   "%u",   "%u"    },
        { sizeof(short),            "S16",  "%d",   "%d"    },
        { sizeof(unsigned short),   "U16",  "%u",   "%u"    },
        { sizeof(int),              "S32",  "%d",   "%d"    },
        { sizeof(unsigned int),     "U32",  "%u",   "%u"    },
#ifdef _MSC_VER
        { sizeof(ImS64),            "S64",  "%I64d","%I64d" },
        { sizeof(ImU64),            "U64",  "%I64u","%I64u" },
#else
        { sizeof(ImS64),            "S64",  "%lld", "%lld"  },
{ sizeof(ImU64),            "U64",  "%llu", "%llu"  },
#endif
        { sizeof(float),            "float", "%.3f","%f"    },
        { sizeof(double),           "double","%f",  "%lf"   },
    };

    static const std::string VECTOR_NAMES[] = { "X", "Y", "Z", "W" };
    static const std::string VECTOR_IDS[] = { "##X", "##Y", "##Z", "##W" };
    static const std::string VECTOR_COLUMN_IDS[] = { "##col_vX", "##col_vY", "##col_vZ", "##col_vW" };

    const char* DrawKeyGen(const char* label);
    const char* PointerToID(const void* ptr);
    void DrawLabel(const char* label, uint32_t columnIndex);
    bool DrawScalar(const char* label, ImGuiDataType data_type, void* v, float speed = 0.1f, const void* from = nullptr, const void* to = nullptr, const char* format = "%.2f", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawVectorN(const char* label, ImGuiDataType data_type, int size, void* vec, float speed = 0.1f, void* from = nullptr, void* to = nullptr, const void* resetValue = nullptr, const char* format = "%.2f", DrawStateFlag flags = DrawStateFlag::None);

    bool DrawScalarSlider(const char* label, ImGuiDataType data_type, void* v, const void* from = nullptr, const void* to = nullptr, const char* format = "%.2f", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawVectorNSlider(const char* label, ImGuiDataType data_type, int size, void* vec, void* from = nullptr, void* to = nullptr, const void* resetValue = nullptr, const char* format = "%.2f", DrawStateFlag flags = DrawStateFlag::None);
}

#endif //HUTILS_H
