#include "heditorinternal.h"
#include "hyperfloweditor.h"
#include "hutils.h"

namespace hf::editor
{
    bool Draw(const char* label, bool& v, DrawStateFlag flags)
    {
        bool valueChanged = false;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
        if (!(uint32_t)(flags & DrawStateFlag::Nameless)) DrawLabel(label, 0);

        valueChanged = ImGui::Checkbox(DrawKeyGen(label), &v);

        ImGui::PopStyleVar();
        return valueChanged;
    }

    bool DrawString(const char* label, std::string& v, DrawStateFlag flags)
    {
        static std::string old_String;
        bool valueChanged = false;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
        if (!(uint32_t)(flags & DrawStateFlag::Nameless)) DrawLabel(label, 0);

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        valueChanged = ImGui::InputText(DrawKeyGen(label), &v[0], 128);
        ImGui::PopStyleVar();
        return valueChanged;
    }
    bool DrawStringBox(const char* label, std::string& v, DrawStateFlag flags)
    {
        static std::string old_String;
        bool valueChanged = false;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
        if (!(uint32_t)(flags & DrawStateFlag::Nameless)) DrawLabel(label, 0);

        valueChanged = ImGui::InputTextMultiline(DrawKeyGen(label), &v[0], 4096,
                                     { ImGui::GetContentRegionAvail().x, 100.0f });
        ImGui::PopStyleVar();
        return valueChanged;
    }

    bool Draw(const char* label, int8_t& v,    DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_S8,     &v, 1.0f, nullptr, nullptr, nullptr, flags); }
    bool Draw(const char* label, uint8_t& v,   DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_U8,     &v, 1.0f, nullptr, nullptr, nullptr, flags); }
    bool Draw(const char* label, int16_t& v,   DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_S16,    &v, 1.0f, nullptr, nullptr, nullptr, flags); }
    bool Draw(const char* label, uint16_t& v,  DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_U16,    &v, 1.0f, nullptr, nullptr, nullptr, flags); }
    bool Draw(const char* label, int32_t& v,   DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_S32,    &v, 1.0f, nullptr, nullptr, nullptr, flags); }
    bool Draw(const char* label, uint32_t& v,  DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_U32,    &v, 1.0f, nullptr, nullptr, nullptr, flags); }
    bool Draw(const char* label, int64_t & v,  DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_S64,    &v, 1.0f, nullptr, nullptr, nullptr, flags); }
    bool Draw(const char* label, uint64_t & v, DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_U64,    &v, 1.0f, nullptr, nullptr, nullptr, flags); }
    bool Draw(const char* label, float_t& v,   DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_Float,  &v, 0.1f, nullptr, nullptr, nullptr, flags); }
    bool Draw(const char* label, double_t& v,  DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_Double, &v, 0.1f, nullptr, nullptr, nullptr, flags); }

    bool Draw(const char* label, i8vec2& vec, int8_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S8, 2, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, i8vec3& vec, int8_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S8, 3, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, i8vec4& vec, int8_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S8, 4, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }

    bool Draw(const char* label, u8vec2& vec, uint8_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U8, 2, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, u8vec3& vec, uint8_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U8, 3, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, u8vec4& vec, uint8_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U8, 4, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }

    bool Draw(const char* label, i16vec2& vec, int16_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S16, 2, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, i16vec3& vec, int16_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S16, 3, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, i16vec4& vec, int16_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S16, 4, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }

    bool Draw(const char* label, u16vec2& vec, uint16_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U16, 2, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, u16vec3& vec, uint16_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U16, 3, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, u16vec4& vec, uint16_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U16, 4, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }

    bool Draw(const char* label, ivec2& vec, int32_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S32, 2, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, ivec3& vec, int32_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S32, 3, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, ivec4& vec, int32_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S32, 4, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }

    bool Draw(const char* label, uvec2& vec, uint32_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U32, 2, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, uvec3& vec, uint32_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U32, 3, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, uvec4& vec, uint32_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U32, 4, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }

    bool Draw(const char* label, i64vec2& vec, int64_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S64, 2, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, i64vec3& vec, int64_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S64, 3, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, i64vec4& vec, int64_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S64, 4, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }

    bool Draw(const char* label, u64vec2& vec, uint64_t resetValue, DrawStateFlag flags)  { return DrawVectorN(label, ImGuiDataType_U64, 2, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, u64vec3& vec, uint64_t resetValue, DrawStateFlag flags)  { return DrawVectorN(label, ImGuiDataType_U64, 3, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, u64vec4& vec, uint64_t resetValue, DrawStateFlag flags)  { return DrawVectorN(label, ImGuiDataType_U64, 4, &vec.x, 1.0f, nullptr, nullptr, &resetValue, nullptr, flags); }

    bool Draw(const char* label, vec2& vec, float_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_Float, 2, &vec.x, 0.1f, nullptr, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, vec3& vec, float_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_Float, 3, &vec.x, 0.1f, nullptr, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, vec4& vec, float_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_Float, 4, &vec.x, 0.1f, nullptr, nullptr, &resetValue, nullptr, flags); }

    bool Draw(const char* label, dvec2& vec, double_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_Double, 2, &vec.x, 0.1f, nullptr, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, dvec3& vec, double_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_Double, 3, &vec.x, 0.1f, nullptr, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, dvec4& vec, double_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_Double, 4, &vec.x, 0.1f, nullptr, nullptr, &resetValue, nullptr, flags); }

    bool DrawColor(const char* label, vec4& vec, DrawStateFlag flags)
    {
        bool valueChanged = false;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

        if (!(uint32_t)(flags & DrawStateFlag::Nameless)) DrawLabel(label, 0);

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        valueChanged = ImGui::ColorEdit4(label, &vec[0],
                                         ImGuiColorEditFlags_NoLabel |
                                         ImGuiColorEditFlags_NoSidePreview);

        ImGui::PopStyleVar();

        ImGui::Spacing();
        return valueChanged;
    }
    bool DrawColor(const char* label, vec3& vec, DrawStateFlag flags)
    {
        bool valueChanged = false;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

        if (!(uint32_t)(flags & DrawStateFlag::Nameless)) DrawLabel(label, 0);

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        valueChanged = ImGui::ColorEdit3(label, &vec[0],
                                         ImGuiColorEditFlags_NoLabel |
                                         ImGuiColorEditFlags_NoSidePreview);

        ImGui::PopStyleVar();

        ImGui::Spacing();
        return valueChanged;
    }

    bool Draw(const char* label, int8_t& v,   int8_t from,   DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_S8,     &v, 1.0f, &from, nullptr, nullptr, flags); }
    bool Draw(const char* label, uint8_t& v,  uint8_t from,  DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_U8,     &v, 1.0f, &from, nullptr, nullptr, flags); }
    bool Draw(const char* label, int16_t& v,  int16_t from,  DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_S16,    &v, 1.0f, &from, nullptr, nullptr, flags); }
    bool Draw(const char* label, uint16_t& v, uint16_t from, DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_U16,    &v, 1.0f, &from, nullptr, nullptr, flags); }
    bool Draw(const char* label, int32_t& v,  int32_t from,  DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_S32,    &v, 1.0f, &from, nullptr, nullptr, flags); }
    bool Draw(const char* label, uint32_t& v, uint32_t from, DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_U32,    &v, 1.0f, &from, nullptr, nullptr, flags); }
    bool Draw(const char* label, int64_t& v,  int64_t from,  DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_S64,    &v, 1.0f, &from, nullptr, nullptr, flags); }
    bool Draw(const char* label, uint64_t& v, uint64_t from, DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_U64,    &v, 1.0f, &from, nullptr, nullptr, flags); }
    bool Draw(const char* label, float_t& v,  float_t from,  DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_Float,  &v, 0.1f, &from, nullptr, nullptr, flags); }
    bool Draw(const char* label, double_t& v, double_t from, DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_Double, &v, 0.1f, &from, nullptr, nullptr, flags); }

    bool Draw(const char* label, i8vec2& vec, i8vec2 from, int8_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S8, 2, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, i8vec3& vec, i8vec3 from, int8_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S8, 3, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, i8vec4& vec, i8vec4 from, int8_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S8, 4, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }

    bool Draw(const char* label, u8vec2& vec, u8vec2 from, uint8_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U8, 2, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, u8vec3& vec, u8vec3 from, uint8_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U8, 3, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, u8vec4& vec, u8vec4 from, uint8_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U8, 4, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }

    bool Draw(const char* label, i16vec2& vec, i16vec2 from, int16_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S16, 2, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, i16vec3& vec, i16vec3 from, int16_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S16, 3, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, i16vec4& vec, i16vec4 from, int16_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S16, 4, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }

    bool Draw(const char* label, u16vec2& vec, u16vec2 from, uint16_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U16, 2, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, u16vec3& vec, u16vec3 from, uint16_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U16, 3, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, u16vec4& vec, u16vec4 from, uint16_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U16, 4, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }

    bool Draw(const char* label, ivec2& vec, ivec2 from, int32_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S32, 2, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, ivec3& vec, ivec3 from, int32_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S32, 3, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, ivec4& vec, ivec4 from, int32_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S32, 4, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }

    bool Draw(const char* label, uvec2& vec, uvec2 from, uint32_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U32, 2, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, uvec3& vec, uvec3 from, uint32_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U32, 3, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, uvec4& vec, uvec4 from, uint32_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U32, 4, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }

    bool Draw(const char* label, i64vec2& vec, i64vec2 from, int64_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S64, 2, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, i64vec3& vec, i64vec3 from, int64_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S64, 3, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, i64vec4& vec, i64vec4 from, int64_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S64, 4, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }

    bool Draw(const char* label, u64vec2& vec, u64vec2 from, uint64_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U64, 2, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, u64vec3& vec, u64vec3 from, uint64_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U64, 3, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, u64vec4& vec, u64vec4 from, uint64_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U64, 4, &vec.x, 1.0f, &from, nullptr, &resetValue, nullptr, flags); }

    bool Draw(const char* label, vec2& vec, vec2 from, float_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_Float, 2, &vec.x, 0.1f, &from, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, vec3& vec, vec3 from, float_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_Float, 3, &vec.x, 0.1f, &from, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, vec4& vec, vec4 from, float_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_Float, 4, &vec.x, 0.1f, &from, nullptr, &resetValue, nullptr, flags); }

    bool Draw(const char* label, dvec2& vec, dvec2 from, double_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_Double, 2, &vec.x, 0.1f, &from, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, dvec3& vec, dvec3 from, double_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_Double, 3, &vec.x, 0.1f, &from, nullptr, &resetValue, nullptr, flags); }
    bool Draw(const char* label, dvec4& vec, dvec4 from, double_t resetValue, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_Double, 4, &vec.x, 0.1f, &from, nullptr, &resetValue, nullptr, flags); }

    bool Draw(const char* label, int8_t& v,    int8_t from,   int8_t to,   const char* format, DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_S8,     &v, 1.0f, &from, &to, format, flags); }
    bool Draw(const char* label, uint8_t& v,   uint8_t from,  uint8_t to,  const char* format, DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_U8,     &v, 1.0f, &from, &to, format, flags); }
    bool Draw(const char* label, int16_t& v,   int16_t from,  int16_t to,  const char* format, DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_S16,    &v, 1.0f, &from, &to, format, flags); }
    bool Draw(const char* label, uint16_t& v,  uint16_t from, uint16_t to, const char* format, DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_U16,    &v, 1.0f, &from, &to, format, flags); }
    bool Draw(const char* label, int32_t& v,   int32_t from,  int32_t to,  const char* format, DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_S32,    &v, 1.0f, &from, &to, format, flags); }
    bool Draw(const char* label, uint32_t& v,  uint32_t from, uint32_t to, const char* format, DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_U32,    &v, 1.0f, &from, &to, format, flags); }
    bool Draw(const char* label, int64_t & v,  int64_t from,  int64_t to,  const char* format, DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_S64,    &v, 1.0f, &from, &to, format, flags); }
    bool Draw(const char* label, uint64_t & v, uint64_t from, uint64_t to, const char* format, DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_U64,    &v, 1.0f, &from, &to, format, flags); }
    bool Draw(const char* label, float_t& v,   float_t from,  float_t to,  const char* format, DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_Float,  &v, 0.1f, &from, &to, format, flags); }
    bool Draw(const char* label, double_t& v,  double_t from, double_t to, const char* format, DrawStateFlag flags) { return DrawScalar(label, ImGuiDataType_Double, &v, 0.1f, &from, &to, format, flags); }

    bool Draw(const char* label, i8vec2& vec, i8vec2 from, i8vec2 to, int8_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S8, 2, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }
    bool Draw(const char* label, i8vec3& vec, i8vec3 from, i8vec3 to, int8_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S8, 3, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }
    bool Draw(const char* label, i8vec4& vec, i8vec4 from, i8vec4 to, int8_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S8, 4, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }

    bool Draw(const char* label, u8vec2& vec, u8vec2 from, u8vec2 to, uint8_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U8, 2, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }
    bool Draw(const char* label, u8vec3& vec, u8vec3 from, u8vec3 to, uint8_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U8, 3, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }
    bool Draw(const char* label, u8vec4& vec, u8vec4 from, u8vec4 to, uint8_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U8, 4, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }

    bool Draw(const char* label, i16vec2& vec, i16vec2 from, i16vec2 to, int16_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S16, 2, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }
    bool Draw(const char* label, i16vec3& vec, i16vec3 from, i16vec3 to, int16_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S16, 3, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }
    bool Draw(const char* label, i16vec4& vec, i16vec4 from, i16vec4 to, int16_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S16, 4, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }

    bool Draw(const char* label, u16vec2& vec, u16vec2 from, u16vec2 to, uint16_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U16, 2, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }
    bool Draw(const char* label, u16vec3& vec, u16vec3 from, u16vec3 to, uint16_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U16, 3, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }
    bool Draw(const char* label, u16vec4& vec, u16vec4 from, u16vec4 to, uint16_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U16, 4, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }

    bool Draw(const char* label, ivec2& vec, ivec2 from, ivec2 to, int32_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S32, 2, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }
    bool Draw(const char* label, ivec3& vec, ivec3 from, ivec3 to, int32_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S32, 3, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }
    bool Draw(const char* label, ivec4& vec, ivec4 from, ivec4 to, int32_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S32, 4, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }

    bool Draw(const char* label, uvec2& vec, uvec2 from, uvec2 to, uint32_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U32, 2, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }
    bool Draw(const char* label, uvec3& vec, uvec3 from, uvec3 to, uint32_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U32, 3, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }
    bool Draw(const char* label, uvec4& vec, uvec4 from, uvec4 to, uint32_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U32, 4, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }

    bool Draw(const char* label, i64vec2& vec, i64vec2 from, i64vec2 to, int64_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S64, 2, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }
    bool Draw(const char* label, i64vec3& vec, i64vec3 from, i64vec3 to, int64_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S64, 3, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }
    bool Draw(const char* label, i64vec4& vec, i64vec4 from, i64vec4 to, int64_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_S64, 4, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }

    bool Draw(const char* label, u64vec2& vec, u64vec2 from, u64vec2 to, uint64_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U64, 2, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }
    bool Draw(const char* label, u64vec3& vec, u64vec3 from, u64vec3 to, uint64_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U64, 3, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }
    bool Draw(const char* label, u64vec4& vec, u64vec4 from, u64vec4 to, uint64_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_U64, 4, &vec.x, 1.0f, &from, &to, &resetValue, format, flags); }

    bool Draw(const char* label, vec2& vec, vec2 from, vec2 to, float_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_Float, 2, &vec.x, 0.1f, &from, &to, &resetValue, format, flags); }
    bool Draw(const char* label, vec3& vec, vec3 from, vec3 to, float_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_Float, 3, &vec.x, 0.1f, &from, &to, &resetValue, format, flags); }
    bool Draw(const char* label, vec4& vec, vec4 from, vec4 to, float_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_Float, 4, &vec.x, 0.1f, &from, &to, &resetValue, format, flags); }

    bool Draw(const char* label, dvec2& vec, dvec2 from, dvec2 to, double_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_Double, 2, &vec.x, 0.1f, &from, &to, &resetValue, format, flags); }
    bool Draw(const char* label, dvec3& vec, dvec3 from, dvec3 to, double_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_Double, 3, &vec.x, 0.1f, &from, &to, &resetValue, format, flags); }
    bool Draw(const char* label, dvec4& vec, dvec4 from, dvec4 to, double_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorN(label, ImGuiDataType_Double, 4, &vec.x, 0.1f, &from, &to, &resetValue, format, flags); }

    bool DrawSlider(const char* label, int8_t& v,   int8_t from,   int8_t to,   const char* format, DrawStateFlag flags) { return DrawScalarSlider(label, ImGuiDataType_S8,     &v, &from, &to, format, flags); }
    bool DrawSlider(const char* label, uint8_t& v,  uint8_t from,  uint8_t to,  const char* format, DrawStateFlag flags) { return DrawScalarSlider(label, ImGuiDataType_U8,     &v, &from, &to, format, flags); }
    bool DrawSlider(const char* label, int16_t& v,  int16_t from,  int16_t to,  const char* format, DrawStateFlag flags) { return DrawScalarSlider(label, ImGuiDataType_S16,    &v, &from, &to, format, flags); }
    bool DrawSlider(const char* label, uint16_t& v, uint16_t from, uint16_t to, const char* format, DrawStateFlag flags) { return DrawScalarSlider(label, ImGuiDataType_U16,    &v, &from, &to, format, flags); }
    bool DrawSlider(const char* label, int32_t& v,  int32_t from,  int32_t to,  const char* format, DrawStateFlag flags) { return DrawScalarSlider(label, ImGuiDataType_S32,    &v, &from, &to, format, flags); }
    bool DrawSlider(const char* label, uint32_t& v, uint32_t from, uint32_t to, const char* format, DrawStateFlag flags) { return DrawScalarSlider(label, ImGuiDataType_U32,    &v, &from, &to, format, flags); }
    bool DrawSlider(const char* label, int64_t& v,  int64_t from,  int64_t to,  const char* format, DrawStateFlag flags) { return DrawScalarSlider(label, ImGuiDataType_S64,    &v, &from, &to, format, flags); }
    bool DrawSlider(const char* label, uint64_t& v, uint64_t from, uint64_t to, const char* format, DrawStateFlag flags) { return DrawScalarSlider(label, ImGuiDataType_U64,    &v, &from, &to, format, flags); }
    bool DrawSlider(const char* label, float_t& v,  float_t from,  float_t to,  const char* format, DrawStateFlag flags) { return DrawScalarSlider(label, ImGuiDataType_Float,  &v, &from, &to, format, flags); }
    bool DrawSlider(const char* label, double_t& v, double_t from, double_t to, const char* format, DrawStateFlag flags) { return DrawScalarSlider(label, ImGuiDataType_Double, &v, &from, &to, format, flags); }

    bool DrawSlider(const char* label, i8vec2& vec, i8vec2 from, i8vec2 to, int8_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_S8, 2, &vec.x, &from, &to, &resetValue, format, flags); }
    bool DrawSlider(const char* label, i8vec3& vec, i8vec3 from, i8vec3 to, int8_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_S8, 3, &vec.x, &from, &to, &resetValue, format, flags); }
    bool DrawSlider(const char* label, i8vec4& vec, i8vec4 from, i8vec4 to, int8_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_S8, 4, &vec.x, &from, &to, &resetValue, format, flags); }

    bool DrawSlider(const char* label, u8vec2& vec, u8vec2 from, u8vec2 to, uint8_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_U8, 2, &vec.x, &from, &to, &resetValue, format, flags); }
    bool DrawSlider(const char* label, u8vec3& vec, u8vec3 from, u8vec3 to, uint8_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_U8, 3, &vec.x, &from, &to, &resetValue, format, flags); }
    bool DrawSlider(const char* label, u8vec4& vec, u8vec4 from, u8vec4 to, uint8_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_U8, 4, &vec.x, &from, &to, &resetValue, format, flags); }

    bool DrawSlider(const char* label, i16vec2& vec, i16vec2 from, i16vec2 to, int16_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_S16, 2, &vec.x, &from, &to, &resetValue, format, flags); }
    bool DrawSlider(const char* label, i16vec3& vec, i16vec3 from, i16vec3 to, int16_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_S16, 3, &vec.x, &from, &to, &resetValue, format, flags); }
    bool DrawSlider(const char* label, i16vec4& vec, i16vec4 from, i16vec4 to, int16_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_S16, 4, &vec.x, &from, &to, &resetValue, format, flags); }

    bool DrawSlider(const char* label, u16vec2& vec, u16vec2 from, u16vec2 to, uint16_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_U16, 2, &vec.x, &from, &to, &resetValue, format, flags); }
    bool DrawSlider(const char* label, u16vec3& vec, u16vec3 from, u16vec3 to, uint16_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_U16, 3, &vec.x, &from, &to, &resetValue, format, flags); }
    bool DrawSlider(const char* label, u16vec4& vec, u16vec4 from, u16vec4 to, uint16_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_U16, 4, &vec.x, &from, &to, &resetValue, format, flags); }

    bool DrawSlider(const char* label, ivec2& vec, ivec2 from, ivec2 to, int32_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_S32, 2, &vec.x, &from, &to, &resetValue, format, flags); }
    bool DrawSlider(const char* label, ivec3& vec, ivec3 from, ivec3 to, int32_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_S32, 3, &vec.x, &from, &to, &resetValue, format, flags); }
    bool DrawSlider(const char* label, ivec4& vec, ivec4 from, ivec4 to, int32_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_S32, 4, &vec.x, &from, &to, &resetValue, format, flags); }

    bool DrawSlider(const char* label, uvec2& vec, uvec2 from, uvec2 to, uint32_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_U32, 2, &vec.x, &from, &to, &resetValue, format, flags); }
    bool DrawSlider(const char* label, uvec3& vec, uvec3 from, uvec3 to, uint32_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_U32, 3, &vec.x, &from, &to, &resetValue, format, flags); }
    bool DrawSlider(const char* label, uvec4& vec, uvec4 from, uvec4 to, uint32_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_U32, 4, &vec.x, &from, &to, &resetValue, format, flags); }

    bool DrawSlider(const char* label, i64vec2& vec, i64vec2 from, i64vec2 to, int64_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_S64, 2, &vec.x, &from, &to, &resetValue, format, flags); }
    bool DrawSlider(const char* label, i64vec3& vec, i64vec3 from, i64vec3 to, int64_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_S64, 3, &vec.x, &from, &to, &resetValue, format, flags); }
    bool DrawSlider(const char* label, i64vec4& vec, i64vec4 from, i64vec4 to, int64_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_S64, 4, &vec.x, &from, &to, &resetValue, format, flags); }

    bool DrawSlider(const char* label, u64vec2& vec, u64vec2 from, u64vec2 to, uint64_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_U64, 2, &vec.x, &from, &to, &resetValue, format, flags); }
    bool DrawSlider(const char* label, u64vec3& vec, u64vec3 from, u64vec3 to, uint64_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_U64, 3, &vec.x, &from, &to, &resetValue, format, flags); }
    bool DrawSlider(const char* label, u64vec4& vec, u64vec4 from, u64vec4 to, uint64_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_U64, 4, &vec.x, &from, &to, &resetValue, format, flags); }

    bool DrawSlider(const char* label, vec2& vec, vec2 from, vec2 to, float_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_Float, 2, &vec.x, &from, &to, &resetValue, format, flags); }
    bool DrawSlider(const char* label, vec3& vec, vec3 from, vec3 to, float_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_Float, 3, &vec.x, &from, &to, &resetValue, format, flags); }
    bool DrawSlider(const char* label, vec4& vec, vec4 from, vec4 to, float_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_Float, 4, &vec.x, &from, &to, &resetValue, format, flags); }

    bool DrawSlider(const char* label, dvec2& vec, dvec2 from, dvec2 to, double_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_Double, 2, &vec.x, &from, &to, &resetValue, format, flags); }
    bool DrawSlider(const char* label, dvec3& vec, dvec3 from, dvec3 to, double_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_Double, 3, &vec.x, &from, &to, &resetValue, format, flags); }
    bool DrawSlider(const char* label, dvec4& vec, dvec4 from, dvec4 to, double_t resetValue, const char* format, DrawStateFlag flags) { return DrawVectorNSlider(label, ImGuiDataType_Double, 4, &vec.x, &from, &to, &resetValue, format, flags); }
}