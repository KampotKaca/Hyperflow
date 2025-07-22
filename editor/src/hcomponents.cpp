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
    static bool DrawCone(AudioCone& cone, DrawStateFlag flags);

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

    bool Draw(const char* label, const Ref<AudioPlayer>& pl, DrawStateFlag flags)
    {
        return DrawComponent(label, [&]
        {
            DrawAudioSettings(pl, flags);
        });
    }

    bool Draw(const char* label, const Ref<AudioPlayer3D>& pl, DrawStateFlag flags)
    {
        return DrawComponent(label, [&]
        {
            vec2 range = GetRange(pl);
            vec2 oldDistance = range;
            Draw("Range", range.x, 0.01f, 100000.0f, "%.2f", flags | DrawStateFlag::DontStretchWidth);
            ImGui::SameLine();
            DrawSlider("Falloff", range.y, 0.0f, range.x, "%.2f", flags | DrawStateFlag::Nameless);

            if (range != oldDistance) SetRange(pl, range.x, range.y);

            DrawAudioSettings(pl, flags);
            auto cone = GetCone(pl);
            if(DrawCone(cone, flags)) Set(pl, cone);
            auto velocity = GetVelocity(pl);
            if (Draw("Velocity", velocity, 0, flags | DrawStateFlag::ButtonLess)) SetVelocity(pl, velocity);
        });
    }

    bool Draw(const char* label, const Ref<AudioListener>& ls, DrawStateFlag flags)
    {
        return DrawComponent(label, [&]
        {
            auto isEnabled = IsEnabled(ls);
            if (Draw("Enabled", isEnabled, flags)) Enable(ls, isEnabled);
            auto cone = GetCone(ls);
            if(DrawCone(cone, flags)) Set(ls, cone);
            auto velocity = GetVelocity(ls);
            if (Draw("Velocity", velocity, 0, flags | DrawStateFlag::ButtonLess)) SetVelocity(ls, velocity);
        });
    }

    bool DrawCone(AudioCone& cone, DrawStateFlag flags)
    {
        bool result = false;
        result = DrawSlider("Inner Angle", cone.innerAngle, 0.0f, 360.0f, "%.1f", flags) || result;
        result = DrawSlider("Outer Angle", cone.outerAngle, 0.0f, 360.0f, "%.1f", flags) || result;
        result = DrawSlider("Outer Gain", cone.outerGain, 0.0f, 1.0f, "%.1f", flags) || result;
        result = Draw("Position", cone.position, 0, flags | DrawStateFlag::ButtonLess) || result;
        result = Draw("Rotation", cone.euler, 0, flags | DrawStateFlag::ButtonLess) || result;
        return result;
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