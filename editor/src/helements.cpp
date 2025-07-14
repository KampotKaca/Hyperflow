#include "heditorinternal.h"
#include "hyperfloweditor.h"
#include "hutils.h"

namespace hf::editor
{
    bool Draw(const char* label, bool& v)
    {
        bool valueChanged = false;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
        VAR_NAME

        valueChanged = ImGui::Checkbox(DrawKeyGen(label), &v);

        ImGui::PopStyleVar();
        return valueChanged;
    }

    bool DrawString(const char* label, std::string& v)
    {
        static std::string old_String;
        bool valueChanged = false;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
        VAR_NAME

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        valueChanged = ImGui::InputText(DrawKeyGen(label), &v[0], 128);
        ImGui::PopStyleVar();
        return valueChanged;
    }
    bool DrawStringBox(const char* label, std::string& v)
    {
        static std::string old_String;
        bool valueChanged = false;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
        VAR_NAME

        valueChanged = ImGui::InputTextMultiline(DrawKeyGen(label), &v[0], 4096,
                                     { ImGui::GetContentRegionAvail().x, 100.0f });
        ImGui::PopStyleVar();
        return valueChanged;
    }

    bool Draw(const char* label, int16_t& v)   { return DrawScalar(label, ImGuiDataType_S16,    &v); }
    bool Draw(const char* label, uint16_t& v)  { return DrawScalar(label, ImGuiDataType_U16,    &v); }
    bool Draw(const char* label, int32_t& v)   { return DrawScalar(label, ImGuiDataType_S32,    &v); }
    bool Draw(const char* label, uint32_t& v)  { return DrawScalar(label, ImGuiDataType_U32,    &v); }
    bool Draw(const char* label, int64_t & v)  { return DrawScalar(label, ImGuiDataType_S64,    &v); }
    bool Draw(const char* label, uint64_t & v) { return DrawScalar(label, ImGuiDataType_U64,    &v); }
    bool Draw(const char* label, float_t& v)   { return DrawScalar(label, ImGuiDataType_Float,  &v); }
    bool Draw(const char* label, double_t& v)  { return DrawScalar(label, ImGuiDataType_Double, &v); }

    bool Draw(const char* label, vec2& vec, float_t resetValue) { return DrawVectorN(label, ImGuiDataType_Float, 2, &vec.x, 0.1f, nullptr, nullptr, &resetValue); }
    bool Draw(const char* label, vec3& vec, float_t resetValue) { return DrawVectorN(label, ImGuiDataType_Float, 3, &vec.x, 0.1f, nullptr, nullptr, &resetValue); }
    bool Draw(const char* label, vec4& vec, float_t resetValue) { return DrawVectorN(label, ImGuiDataType_Float, 4, &vec.x, 0.1f, nullptr, nullptr, &resetValue); }

    bool Draw(const char* label, ivec2& vec, int32_t resetValue) { return DrawVectorN(label, ImGuiDataType_S32, 2, &vec.x, 0.1f, nullptr, nullptr, &resetValue); }
    bool Draw(const char* label, ivec3& vec, int32_t resetValue) { return DrawVectorN(label, ImGuiDataType_S32, 3, &vec.x, 0.1f, nullptr, nullptr, &resetValue); }
    bool Draw(const char* label, ivec4& vec, int32_t resetValue) { return DrawVectorN(label, ImGuiDataType_S32, 4, &vec.x, 0.1f, nullptr, nullptr, &resetValue); }

    bool DrawColor(const char* label, vec4& vec)
    {
        bool valueChanged = false;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
        VAR_NAME

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        valueChanged = ImGui::ColorEdit4(label, &vec[0],
                                         ImGuiColorEditFlags_NoLabel |
                                         ImGuiColorEditFlags_NoSidePreview);

        ImGui::PopStyleVar();

        ImGui::Spacing();
        return valueChanged;
    }
    bool DrawColor(const char* label, vec3& vec)
    {
        bool valueChanged = false;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
        VAR_NAME

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        valueChanged = ImGui::ColorEdit3(label, &vec[0],
                                         ImGuiColorEditFlags_NoLabel |
                                         ImGuiColorEditFlags_NoSidePreview);

        ImGui::PopStyleVar();

        ImGui::Spacing();
        return valueChanged;
    }

    bool Draw(const char* label, int16_t& v, int16_t from, int16_t to)     { return DrawScalar(label, ImGuiDataType_S16, &v, 0.1f, &from, &to); }
    bool Draw(const char* label, uint16_t& v, uint16_t from, uint16_t to)  { return DrawScalar(label, ImGuiDataType_U16, &v, 0.1f, &from, &to); }
    bool Draw(const char* label, int32_t& v, int32_t from, int32_t to)     { return DrawScalar(label, ImGuiDataType_S32, &v, 0.1f, &from, &to); }
    bool Draw(const char* label, uint32_t& v, uint32_t from, uint32_t to)  { return DrawScalar(label, ImGuiDataType_U32, &v, 0.1f, &from, &to); }
    bool Draw(const char* label, int64_t & v, int64_t from, int64_t to)    { return DrawScalar(label, ImGuiDataType_S64, &v, 0.1f, &from, &to); }
    bool Draw(const char* label, uint64_t & v, uint64_t from, uint64_t to) { return DrawScalar(label, ImGuiDataType_U64, &v, 0.1f, &from, &to); }
    bool Draw(const char* label, float_t& v, float_t from, float_t to)     { return DrawScalar(label, ImGuiDataType_Float, &v, 0.1f, &from, &to); }
    bool Draw(const char* label, double_t& v, double_t from, double_t to)  { return DrawScalar(label, ImGuiDataType_Double, &v, 0.1f, &from, &to); }

    bool Draw(const char* label, vec2& vec, vec2 from, vec2 to, float_t resetValue) { return DrawVectorN(label, ImGuiDataType_Float, 2, &vec.x, 0.1f, &from, &to, &resetValue); }
    bool Draw(const char* label, vec3& vec, vec3 from, vec3 to, float_t resetValue) { return DrawVectorN(label, ImGuiDataType_Float, 3, &vec.x, 0.1f, &from, &to, &resetValue); }
    bool Draw(const char* label, vec4& vec, vec4 from, vec4 to, float_t resetValue) { return DrawVectorN(label, ImGuiDataType_Float, 4, &vec.x, 0.1f, &from, &to, &resetValue); }

    bool Draw(const char* label, ivec2& vec, ivec2 from, ivec2 to, int32_t resetValue) { return DrawVectorN(label, ImGuiDataType_S32, 2, &vec.x, 0.1f, &from, &to, &resetValue); }
    bool Draw(const char* label, ivec3& vec, ivec3 from, ivec3 to, int32_t resetValue) { return DrawVectorN(label, ImGuiDataType_S32, 3, &vec.x, 0.1f, &from, &to, &resetValue); }
    bool Draw(const char* label, ivec4& vec, ivec4 from, ivec4 to, int32_t resetValue) { return DrawVectorN(label, ImGuiDataType_S32, 4, &vec.x, 0.1f, &from, &to, &resetValue); }
}