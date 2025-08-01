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
        auto audioProgress = (uint32_t)(GetPlayedPercent(pl) * 100.0f);
        if(DrawSlider("Progress", audioProgress, 0u, 100u, "%.u%%", DrawStateFlag::Nameless))
        {
            modified = true;
            SeekPercent(pl, (float)audioProgress * 0.01f);
        }

        return modified;
    }

    template bool DrawAudioSettings(const Ref<AudioPlayer>& pl, DrawStateFlag flags);
    template bool DrawAudioSettings(const Ref<AudioPlayer3D>& pl, DrawStateFlag flags);

    static bool DrawCone(AudioCone& cone, DrawStateFlag flags);
    static bool DrawTable(const char* label);

    bool StartDropdown(const char* label)
    {
        bool result = ImGui::CollapsingHeader(label);
        if (result) ImGui::Indent();
        return result;
    }

    void EndDropdown()
    {
        ImGui::Unindent();
    }

    bool StartComponent(const char* label, DrawStateFlag flags)
    {
        if (!(uint32_t)(flags & DrawStateFlag::DontUseDropdown))
        {
            if (ImGui::CollapsingHeader(label)) return DrawTable(label);
            return false;
        }
        return DrawTable(label);
    }

    void EndComponent()
    {
        ImGui::EndTable();
    }

    bool Draw(const char* label, Transform& trs, DrawStateFlag flags)
    {
        if(StartComponent(label, flags))
        {
            Draw("Position", trs.position, 0, flags);
            Draw("Rotation", trs.euler, 0, flags);
            Draw("Scale", trs.scale, 1, flags);

            EndComponent();
            return true;
        }
        return false;
    }

    bool Draw(const char* label, DirectionalLight& dl, DrawStateFlag flags)
    {
        if(StartComponent(label, flags))
        {
            DrawColor("Color", dl.color, flags);
            Draw("Rotation", dl.euler, 0, flags | DrawStateFlag::ButtonLess);

            EndComponent();
            return true;
        }
        return false;
    }

    bool Draw(const char* label, Camera3DCore& cam, DrawStateFlag flags)
    {
        if(StartComponent(label, flags))
        {
            DrawSlider("Fov", cam.fov, 1.0f, 179.0f, "%.1f", flags);
            Draw("Distance", cam.farPlane, 0.01f, 100000.0f, "%.1f", flags | DrawStateFlag::DontStretchWidth);
            ImGui::SameLine();
            DrawSlider("Near", cam.nearPlane, 0.1f, cam.farPlane, "%.2f", flags | DrawStateFlag::Nameless);
            cam.nearPlane = glm::min(cam.nearPlane, cam.farPlane);

            cam.farPlane = glm::max(cam.nearPlane, cam.farPlane);

            EndComponent();
            return true;
        }
        return false;
    }

    static void DrawThreadStats(const char* label, const ThreadMemoryStatistics& stats)
    {
        if (StartDropdown(label))
        {
            ImGui::Text("Cash Size:               %.1f Mbs", stats.cacheSizeMbs);
            ImGui::Text("Cash Span:               %.1f Mbs", stats.cacheSpanMbs);
            ImGui::Text("Thread To Global:        %.1f Mbs", stats.threadToGlobalMbs);
            ImGui::Text("Global To Thread:        %.1f Mbs", stats.globalToThreadMbs);

            ImGui::Text("Current Num Spans:       %llu", stats.currentNumSpans);
            ImGui::Text("Peak Num Spans:          %llu", stats.peakNumSpans);

            ImGui::Text("Current Num Allocations: %llu", stats.currentNumAllocations);
            ImGui::Text("Peak Num Allocations:    %llu", stats.peakNumAllocations);
            ImGui::Text("Total Num Allocations:   %llu", stats.totalNumAllocations);
            ImGui::Text("Total Num Frees:         %llu", stats.totalNumFrees);

            EndDropdown();
        }
    }

    bool DrawMemoryStatisticsWindow(const char* label, bool* isOpen, const WindowFlags flags)
    {
        if(StartWindow(label, isOpen, flags))
        {
            const auto globalStats = utils::GetGlobalMemoryStatistics();
            ImGui::Text("Mapped:          %.1f Mbs", globalStats.mappedSizeMbs);
            ImGui::Text("Mapped Peak:     %.1f Mbs", globalStats.mappedPeakSizeMbs);
            ImGui::Text("Cached:          %.1f Mbs", globalStats.cachedSizeMbs);
            ImGui::Text("Huge Alloc:      %.1f Mbs", globalStats.hugeAllocSizeMbs);
            ImGui::Text("Huge Alloc Peak: %.1f Mbs", globalStats.hugeAllocPeakSizeMbs);
            ImGui::Text("Mapped Size:     %.1f Mbs", globalStats.mappedTotalSizeMbs);
            ImGui::Text("Unmapped Size:   %.1f Mbs", globalStats.unmappedTotalSizeMbs);

            DrawThreadStats("Update Thread", utils::GetThreadMemoryStatistics());
            DrawThreadStats("Render Thread", GetMemoryStatistics(GetRenderer(GetMainWindow())));

            EndWindow();
            return true;
        }
        return false;
    }

    bool Draw(const char* label, const Ref<AudioPlayer>& pl, DrawStateFlag flags)
    {
        if(StartComponent(label, flags))
        {
            DrawAudioSettings(pl, flags);

            EndComponent();
            return true;
        }
        return false;
    }

    bool Draw(const char* label, const Ref<AudioPlayer3D>& pl, DrawStateFlag flags)
    {
        if(StartComponent(label, flags))
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

            EndComponent();
            return true;
        }
        return false;
    }

    bool Draw(const char* label, const Ref<AudioListener>& ls, DrawStateFlag flags)
    {
        if(StartComponent(label, flags))
        {
            auto isEnabled = IsEnabled(ls);
            if (Draw("Enabled", isEnabled, flags)) Enable(ls, isEnabled);
            auto cone = GetCone(ls);
            if(DrawCone(cone, flags)) Set(ls, cone);
            auto velocity = GetVelocity(ls);
            if (Draw("Velocity", velocity, 0, flags | DrawStateFlag::ButtonLess)) SetVelocity(ls, velocity);

            EndComponent();
            return true;
        }
        return false;
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

    bool DrawTable(const char* label)
    {
        if(ImGui::BeginTable(DrawKeyGen(label), 2))
        {
            ImGui::TableSetupColumn("##col_1", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("##col_2", ImGuiTableColumnFlags_WidthStretch, 70.0f);
            return true;
        }
        return false;
    }
}