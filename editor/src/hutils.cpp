#include "hutils.h"

namespace hf::ed
{
    const char* DrawKeyGen(const char* label)
    {
        static char key[256];
        key[0] = '#';
        key[1] = '#';
        key[2] = '_';
        strcpy(key + 3, label);
        return key;
    }

    const char* PointerToID(const void* ptr)
    {
        static char key[32];
        snprintf(key, sizeof(key), "%lu", (uint64_t)ptr);
        return key;
    }

    void DrawLabel(const char* label, uint32_t columnIndex)
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(columnIndex);
        ImGui::Text("%s", label);
        ImGui::TableNextColumn();
    }

    static bool DrawResetButton(uint32_t size);

    bool DrawScalar(const char* label, ImGuiDataType data_type, void* v, float speed, const void* from, const void* to, const char* format, DrawStateFlag flags)
    {
        bool valueChanged = false;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

        if (!(uint32_t)(flags & DrawStateFlag::Nameless)) DrawLabel(label, 0);
        if (!(uint32_t)(flags & DrawStateFlag::DontStretchWidth)) ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        else ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * .6f);
        valueChanged = ImGui::DragScalar(DrawKeyGen(label), data_type, v, speed, from, to, format);

        ImGui::PopStyleVar();
        return valueChanged;
    }

    bool DrawVectorN(const char* label, ImGuiDataType data_type, int size, void* vec, float speed, void* from, void* to, const void* resetValue, const char* format, DrawStateFlag flags)
    {
        bool valueChanged = false;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

        if (!(uint32_t)(flags & DrawStateFlag::Nameless)) DrawLabel(label, 0);

        if(ImGui::BeginTable(label, size))
        {
            for (int i = 0; i < size; ++i)
                ImGui::TableSetupColumn(VECTOR_COLUMN_IDS[i].c_str(),
                                        ImGuiTableColumnFlags_WidthStretch, 70.0f);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);

            size_t type_size = GDataTypeInfo[data_type].Size;
            void* ptr = vec;
            for (int i = 0; i < size; ++i)
            {
                if (!(uint32_t)(flags & DrawStateFlag::ButtonLess))
                {
                    if (DrawResetButton(i))
                    {
                        std::memcpy(ptr, resetValue, type_size);
                        valueChanged = true;
                    }
                }

                if (!(uint32_t)(flags & DrawStateFlag::DontStretchWidth)) ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                else ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * .6f);

                valueChanged |= DrawScalar(VECTOR_IDS[i].c_str(), data_type, ptr, speed, from, to, format, DrawStateFlag::Nameless);
                ptr = (void*)((char*)ptr + type_size);

                if(i + 1 < size) ImGui::TableNextColumn();
            }

            ImGui::EndTable();
        }
        ImGui::PopStyleVar();

        ImGui::Spacing();
        return valueChanged;
    }

    bool DrawScalarSlider(const char* label, ImGuiDataType data_type, void* v, const void* from, const void* to, const char* format, DrawStateFlag flags)
    {
        bool valueChanged = false;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

        if (!(uint32_t)(flags & DrawStateFlag::Nameless)) DrawLabel(label, 0);

        if (!(uint32_t)(flags & DrawStateFlag::DontStretchWidth)) ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        else ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * .6f);

        valueChanged = ImGui::SliderScalar(DrawKeyGen(label), data_type, v, from, to, format);

        ImGui::PopStyleVar();
        return valueChanged;
    }

    bool DrawVectorNSlider(const char* label, ImGuiDataType data_type, int size, void* vec, void* from, void* to, const void* resetValue, const char* format, DrawStateFlag flags)
    {
        bool valueChanged = false;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
        if (!(uint32_t)(flags & DrawStateFlag::Nameless)) DrawLabel(label, 0);

        if(ImGui::BeginTable(label, size))
        {
            for (int i = 0; i < size; ++i)
                ImGui::TableSetupColumn(VECTOR_COLUMN_IDS[i].c_str(),
                                        ImGuiTableColumnFlags_WidthStretch, 70.0f);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);

            size_t type_size = GDataTypeInfo[data_type].Size;
            void* ptr = vec;
            for (int i = 0; i < size; ++i)
            {
                if (!(uint32_t)(flags & DrawStateFlag::ButtonLess))
                {
                    if (DrawResetButton(i))
                    {
                        std::memcpy(ptr, resetValue, type_size);
                        valueChanged = true;
                    }
                }

                if (!(uint32_t)(flags & DrawStateFlag::DontStretchWidth)) ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                else ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * .6f);

                valueChanged |= DrawScalarSlider(VECTOR_IDS[i].c_str(), data_type, ptr, from, to, format, DrawStateFlag::Nameless);
                ptr = (void*)((char*)ptr + type_size);

                if(i + 1 < size) ImGui::TableNextColumn();
            }

            ImGui::EndTable();
        }
        ImGui::PopStyleVar();

        ImGui::Spacing();
        return valueChanged;
    }

    bool DrawResetButton(uint32_t size)
    {
        ImVec2 buttonSize = { 19, 19 };
        ImGui::PushStyleColor(ImGuiCol_Button, VECTOR_STYLES[size * 3]);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, VECTOR_STYLES[size * 3 + 1]);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, VECTOR_STYLES[size * 3 + 2]);
        bool result = ImGui::Button(VECTOR_NAMES[size].c_str(), buttonSize);
        ImGui::PopStyleColor(3);
        ImGui::SameLine();
        return result;
    }
}