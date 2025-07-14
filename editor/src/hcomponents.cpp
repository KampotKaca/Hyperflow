#include "hyperfloweditor.h"
#include "heditorinternal.h"
#include "hutils.h"

namespace hf::editor
{
    bool DrawComponent(const char* label) { return ImGui::CollapsingHeader(label); }

    bool Draw(const char* label, Transform& trs)
    {
        if(ImGui::BeginTable(DrawKeyGen(label), 2))
        {
            ImGui::TableSetupColumn("##col_1", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("##col_2", ImGuiTableColumnFlags_WidthStretch, 70.0f);

            Draw("Position", trs.position);
            Draw("Rotation", trs.euler);
            Draw("Scale", trs.scale, 1);

            ImGui::EndTable();
            return true;
        }
        return false;
    }

    bool Draw(const char* label, DirectionalLight& dl)
    {
        if(ImGui::BeginTable(DrawKeyGen(label), 2))
        {
            ImGui::TableSetupColumn("##col_1", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("##col_2", ImGuiTableColumnFlags_WidthStretch, 70.0f);

            DrawColor("Color", dl.color);
            Draw("Rotation", dl.euler);

            ImGui::EndTable();
            return true;
        }
        return false;
    }

    bool Draw(const char* label, Camera3DCore& cam)
    {
        if(ImGui::BeginTable(DrawKeyGen(label), 2))
        {
            ImGui::TableSetupColumn("##col_1", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("##col_2", ImGuiTableColumnFlags_WidthStretch, 70.0f);

            Draw("Fov", cam.fov, 1.0f, 179.0f);
            Draw("Near", cam.nearPlane, 0.01f, 1000.0f);
            Draw("Far", cam.farPlane, cam.nearPlane, 10000.0f);

            ImGui::EndTable();
            return true;
        }
        return false;
    }
}