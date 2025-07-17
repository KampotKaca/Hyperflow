#include "hyperfloweditor.h"
#include "heditorinternal.h"
#include "hutils.h"

namespace hf::editor
{
    static bool DrawComponent(const char* label, const std::function<void()>& func);

    bool StartComponent(const char* label)
    {
        bool result = ImGui::CollapsingHeader(label);
        if (result) ImGui::Indent();
        return result;
    }

    void EndComponent()
    {
        ImGui::Unindent();
    }

    bool Draw(const char* label, Transform& trs, DrawStateFlag flags)
    {
        return DrawComponent(label, [&]
        {
            Draw("Position", trs.position, 0, flags);
            Draw("Rotation", trs.euler, 0, flags);
            Draw("Scale", trs.scale, 1, flags);
        });
    }

    bool Draw(const char* label, DirectionalLight& dl, DrawStateFlag flags)
    {
        return DrawComponent(label, [&]
        {
            DrawColor("Color", dl.color, flags);
            Draw("Rotation", dl.euler, 0, flags | DrawStateFlag::ButtonLess);
        });
    }

    bool Draw(const char* label, Camera3DCore& cam, DrawStateFlag flags)
    {
        return DrawComponent(label, [&]
        {
            DrawSlider("Fov", cam.fov, 1.0f, 179.0f, "%.1f", flags);
            Draw("Near", cam.nearPlane, 0.01f, 100000.0f, "%.1f", flags);
            Draw("Far", cam.farPlane, 0.01f, 100000.0f, "%.1f", flags);

            cam.farPlane = glm::max(cam.nearPlane, cam.farPlane);
        });
    }

    bool Draw(const char* label, AudioPlayerConfig& plc, DrawStateFlag flags)
    {
        return DrawComponent(label, [&]
        {
            DrawSlider("Volume", plc.volume, 0.0f, 1.0f, "%.2f", flags);
            DrawSlider("Pitch", plc.pitch, 0.0f, 3.0f, "%.2f", flags);
            Draw("Loop", plc.loopingEnabled, flags);
        });
    }

    bool Draw(const char* label, Ref<AudioPlayer>& pl, DrawStateFlag flags)
    {
        return DrawComponent(label, [&]
        {
            float_t volume = GetVolume(pl);
            float_t pitch = GetPitch(pl);
            bool loop = IsLoopingEnabled(pl);

            if (DrawSlider("Volume", volume, 0.0f, 1.0f, "%.2f", flags)) SetVolume(pl, volume);
            if (DrawSlider("Pitch", pitch, 0.0f, 3.0f, "%.2f", flags)) SetPitch(pl, pitch);
            if (Draw("Loop", loop, flags)) SetLoopingMode(pl, loop);

            ImGui::SameLine();
            if (IsPlaying(pl))
            {
                if (ImGui::Button("Pause")) Pause(pl);
            }
            else
            {
                if(ImGui::Button("Play")) Play(pl);
            }

            ImGui::SameLine();
            uint32_t audioProgress = (uint32_t)(GetPlayedPercent(pl) * 100.0f);
            if(DrawSlider("Progress", audioProgress, 0u, 100u, "%.u%%", DrawStateFlag::Nameless))
                SeekPercent(pl, audioProgress * 0.01f);
        });
    }

    bool Draw(const char* label, Ref<AudioPlayer3D>& pl, DrawStateFlag flags)
    {
        return DrawComponent(label, [&]
        {
            vec3 position = GetPosition(pl);
            vec3 direction = GetDirection(pl);
            float_t minDistance = GetMinDistance(pl);
            float_t maxDistance = GetMaxDistance(pl);
            float_t volume = GetVolume(pl);
            float_t pitch = GetPitch(pl);
            bool loop = IsLoopingEnabled(pl);

            if (Draw("Position", position, 0, flags | DrawStateFlag::ButtonLess)) SetPosition(pl, position);
            if (Draw("Direction", direction, 0, flags | DrawStateFlag::ButtonLess)) SetDirection(pl, direction);

            float_t oldMinDistance = minDistance, oldMaxDistance = maxDistance;
            DrawSlider("Min Distance", minDistance, 0.001f, 100000.0f, "%.2f", flags);
            DrawSlider("Max Distance", maxDistance, 0.001f, 100000.0f, "%.2f", flags);

            maxDistance = glm::max(minDistance, maxDistance);
            if (minDistance != oldMinDistance) SetMinDistance(pl, minDistance);
            if (maxDistance != oldMaxDistance) SetMaxDistance(pl, maxDistance);

            if (DrawSlider("Volume", volume, 0.0f, 1.0f, "%.2f", flags)) SetVolume(pl, volume);
            if (DrawSlider("Pitch", pitch, 0.0f, 3.0f, "%.2f", flags)) SetPitch(pl, pitch);
            if (Draw("Loop", loop, flags)) SetLoopingMode(pl, loop);

            ImGui::SameLine();
            if (IsPlaying(pl))
            {
                if (ImGui::Button("Pause")) Pause(pl);
            }
            else
            {
                if(ImGui::Button("Play")) Play(pl);
            }

            ImGui::SameLine();
            uint32_t audioProgress = (uint32_t)(GetPlayedPercent(pl) * 100.0f);
            if(DrawSlider("Progress", audioProgress, 0u, 100u, "%.u%%", DrawStateFlag::Nameless))
                SeekPercent(pl, audioProgress * 0.01f);
        });
    }

    bool DrawComponent(const char* label, const std::function<void()>& func)
    {
        if (ImGui::CollapsingHeader(label))
        {
            if(ImGui::BeginTable(DrawKeyGen(label), 2))
            {
                ImGui::TableSetupColumn("##col_1", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("##col_2", ImGuiTableColumnFlags_WidthStretch, 70.0f);

                func();

                ImGui::EndTable();
                return true;
            }
        }
        return false;
    }
}