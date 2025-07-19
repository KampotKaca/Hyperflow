#include "hyperfloweditor.h"
#include "heditorinternal.h"
#include "hutils.h"

namespace hf::editor
{
    template<typename T>
    bool DrawAudioSettings(const Ref<T>& pl, DrawStateFlag flags)
    {
        float_t volume = GetVolume(pl);
        float_t pitch = GetPitch(pl);
        bool loop = IsLoopingEnabled(pl);
        bool modified = false;

        if (DrawSlider("Volume", volume, 0.0f, 1.0f, "%.2f", flags))
        {
            SetVolume(pl, volume);
            modified = true;
        }
        if (DrawSlider("Pitch", pitch, 0.0f, 3.0f, "%.2f", flags))
        {
            SetPitch(pl, pitch);
            modified = true;
        }
        if (Draw("Loop", loop, flags))
        {
            SetLoopingMode(pl, loop);
            modified = true;
        }

        ImGui::SameLine();
        if (IsPlaying(pl))
        {
            if (ImGui::Button("Pause"))
            {
                modified = true;
                Pause(pl);
            }
        }
        else
        {
            if(ImGui::Button("Play"))
            {
                modified = true;
                Play(pl);
            }
        }

        ImGui::SameLine();
        uint32_t audioProgress = (uint32_t)(GetPlayedPercent(pl) * 100.0f);
        if(DrawSlider("Progress", audioProgress, 0u, 100u, "%.u%%", DrawStateFlag::Nameless))
        {
            modified = true;
            SeekPercent(pl, audioProgress * 0.01f);
        }

        return modified;
    }

    template bool DrawAudioSettings(const Ref<AudioPlayer>& pl, DrawStateFlag flags);
    template bool DrawAudioSettings(const Ref<AudioPlayer3D>& pl, DrawStateFlag flags);

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
            Draw("Distance", cam.farPlane, 0.01f, 100000.0f, "%.1f", flags | DrawStateFlag::DontStretchWidth);
            ImGui::SameLine();
            DrawSlider("Near", cam.nearPlane, 0.1f, cam.farPlane, "%.2f", flags | DrawStateFlag::Nameless);
            cam.nearPlane = glm::min(cam.nearPlane, cam.farPlane);

            cam.farPlane = glm::max(cam.nearPlane, cam.farPlane);
        });
    }

    bool Draw(const char* label, Ref<AudioPlayer>& pl, DrawStateFlag flags)
    {
        return DrawComponent(label, [&]
        {
            DrawAudioSettings(pl, flags);
        });
    }

    bool Draw(const char* label, Ref<AudioPlayer3D>& pl, DrawStateFlag flags)
    {
        return DrawComponent(label, [&]
        {
            vec2 distance = GetDistance(pl);
            vec2 oldDistance = distance;
            Draw("Distance", distance.x, 0.001f, 100000.0f, "%.2f", flags | DrawStateFlag::DontStretchWidth);
            ImGui::SameLine();
            DrawSlider("Falloff", distance.y, 0.0f, distance.x, "%.2f", flags | DrawStateFlag::Nameless);
            distance.y = glm::min(distance.x, distance.y);

            if (distance != oldDistance) SetDistance(pl, distance.x, distance.y);

            DrawAudioSettings(pl, flags);
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