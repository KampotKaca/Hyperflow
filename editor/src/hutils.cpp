#include "hutils.h"

namespace hf::editor
{
    const char* DrawKeyGen(const char* label)
    {
        static char key[256];
        key[0] = '#';
        key[1] = '#';
        strcpy(key + 2, label);
        return key;
    }

    bool DrawScalar(const char* label, ImGuiDataType data_type, void* v, float speed, void* from, void* to)
    {
        bool valueChanged = false;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
        VAR_NAME

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        valueChanged = ImGui::DragScalar(DrawKeyGen(label), data_type, v, speed, from, to);

        ImGui::PopStyleVar();
        return valueChanged;
    }

    bool DrawNoNameScalar(const char* label, ImGuiDataType data_type, void* v, float speed, void* from, void* to)
    {
        bool valueChanged = false;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if(ImGui::DragScalar(label, data_type, v, speed, from, to)) valueChanged = true;

        ImGui::PopStyleVar();
        return valueChanged;
    }

    bool DrawVectorN(const char* label, ImGuiDataType data_type, int size, void* vec, float speed, void* from, void* to, void* resetValue)
    {
        bool valueChanged = false;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
        VAR_NAME

        if(ImGui::BeginTable(label, size))
        {
            for (int i = 0; i < size; ++i)
                ImGui::TableSetupColumn(VECTOR_COLUMN_IDS[i].c_str(),
                                        ImGuiTableColumnFlags_WidthStretch, 70.0f);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);

            float lineHeight = 18 + GImGui->Style.FramePadding.y * 2.0f;
            ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

            size_t type_size = GDataTypeInfo[data_type].Size;
            void* ptr = vec;
            for (int i = 0; i < size; ++i)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, VECTOR_STYLES[i * 3]);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, VECTOR_STYLES[i * 3 + 1]);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, VECTOR_STYLES[i * 3 + 2]);
                if (ImGui::Button(VECTOR_NAMES[i].c_str(), buttonSize))
                {
                    std::memcpy(ptr, resetValue, type_size);
                    valueChanged = true;
                }
                ImGui::PopStyleColor(3);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                valueChanged |= DrawNoNameScalar(VECTOR_IDS[i].c_str(), data_type, ptr, speed, from, to);
                ptr = (void*)((char*)ptr + type_size);

                if(i + 1 < size) ImGui::TableNextColumn();
            }

            ImGui::EndTable();
        }
        ImGui::PopStyleVar();

        ImGui::Spacing();
        return valueChanged;
    }
}