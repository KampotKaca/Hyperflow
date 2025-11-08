#include "hyperfloweditor.h"
#include "heditorinternal.h"
#include "hyperflow.h"
#include "hutils.h"

namespace hf::ed
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

    static bool DrawCone(AudioCone& cone, DrawStateFlag flags = DrawStateFlag::None);
    static bool DrawTable(const char* label);
    static void DrawCameraCore(Camera3DCore& cam, DrawStateFlag flags = DrawStateFlag::None);
    static void DrawGridLines(GridLinesInfo& info, bool& enabled, DrawStateFlag flags = DrawStateFlag::None);

    bool StartDropdown(const char* label)
    {
        const bool result = ImGui::CollapsingHeader(label);
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
        ImGui::PushID(PointerToID(&trs));
        const bool result = StartComponent(label, flags);
        if(result)
        {
            Draw("Position", trs.position, 0, flags);
            Draw("Rotation", trs.euler, 0, flags);
            Draw("Scale", trs.scale, 1, flags);

            EndComponent();
        }
        ImGui::PopID();
        return result;
    }

    bool Draw(const char* label, DirectionalLight& dl, DrawStateFlag flags)
    {
        ImGui::PushID(PointerToID(&dl));
        const bool result = StartComponent(label, flags);
        if(result)
        {
            DrawColor("Color", dl.color, flags);
            Draw("Rotation", dl.euler, 0, flags | DrawStateFlag::ButtonLess);

            EndComponent();
        }
        ImGui::PopID();
        return result;
    }

    bool Draw(const char* label, Camera3DCore& cam, DrawStateFlag flags)
    {
        ImGui::PushID(PointerToID(&cam));
        const bool result = StartComponent(label, flags);
        if(result)
        {
            DrawCameraCore(cam, flags);
            EndComponent();
        }
        ImGui::PopID();
        return result;
    }

    static void DrawThreadStats(const char* label, const ThreadMemoryStatistics& stats)
    {
        if (StartDropdown(label))
        {
            ImGui::Text("Cash Size:               %.1f Mbs", stats.cacheSizeMbs);
            ImGui::Text("Cash Span:               %.1f Mbs", stats.cacheSpanMbs);
            ImGui::Text("Thread To Global:        %.1f Mbs", stats.threadToGlobalMbs);
            ImGui::Text("Global To Thread:        %.1f Mbs", stats.globalToThreadMbs);

            ImGui::Text("Current Num Spans:       %lu", (uint64_t)stats.currentNumSpans);
            ImGui::Text("Peak Num Spans:          %lu", (uint64_t)stats.peakNumSpans);

            ImGui::Text("Current Num Allocations: %lu", (uint64_t)stats.currentNumAllocations);
            ImGui::Text("Peak Num Allocations:    %lu", (uint64_t)stats.peakNumAllocations);
            ImGui::Text("Total Num Allocations:   %lu", (uint64_t)stats.totalNumAllocations);
            ImGui::Text("Total Num Frees:         %lu", (uint64_t)stats.totalNumFrees);

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
            DrawThreadStats("Render Thread", GetMemoryRendererStatistics());

            EndWindow();
            return true;
        }
        return false;
    }

    bool DrawRenderStatisticsWindow(const char* label, bool* isOpen, WindowFlags flags)
    {
        if(StartWindow(label, isOpen, flags))
        {
            const auto stats = utils::GetRendererStatistics();
            ImGui::Text("Culled DrawCalls: %i", stats.culledDrawCalls);

            EndWindow();
            return true;
        }
        return false;
    }

    bool DrawAudioSettingsWindow(const char* label, bool* isOpen, WindowFlags flags)
    {
        if(StartWindow(label, isOpen, flags))
        {
            if(StartComponent("General Audio Settings", DrawStateFlag::DontUseDropdown))
            {
                float volume = GetAudioVolume();
                if(DrawSlider("Volume", volume, 0.0f, 1.0f, "%.2f")) SetAudioVolume(volume);
                EndComponent();
            }

            Draw("2D Settings", Get2DAudioGroup());
            Draw("3D Settings", Get3DAudioGroup());

            EndWindow();
            return true;
        }
        return false;
    }

    static void SetBelowTheElement(float width)
    {
        ImVec2 pos = ImGui::GetItemRectMin();
        ImVec2 size = ImGui::GetItemRectSize();
        ImVec2 popup_pos(pos.x - (width - size.x) * 0.5f, pos.y + size.y);

        ImGui::SetNextWindowPos(popup_pos);
        ImGui::SetNextWindowSize(ImVec2(width, 0));
    }

    bool DrawEditorWindow(const char* label, EditorContextData& data, bool* isOpen, WindowFlags flags)
    {
        if(StartWindow(label, isOpen, flags))
        {
            ImGui::PushID(PointerToID(&data));
            if (ImGui::Button("Grid")) ImGui::OpenPopup("pop_grid");

            SetBelowTheElement(300.0f);
            if (ImGui::BeginPopup("pop_grid"))
            {
                DrawTable("grid_lines_table");
                DrawGridLines(*data.gridLines, *data.drawGridLines);
                ImGui::EndTable();
                ImGui::EndPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("Cam")) ImGui::OpenPopup("pop_camera");

            SetBelowTheElement(250.0f);
            if (ImGui::BeginPopup("pop_camera"))
            {
                DrawTable("camera");
                DrawCameraCore(*data.camera);
                ImGui::EndTable();
                ImGui::EndPopup();
            }

            ImGui::PopID();
            EndWindow();
            return true;
        }
        return false;
    }

    bool Draw(const char* label, const Ref<AudioPlayer>& pl, DrawStateFlag flags)
    {
        ImGui::PushID(PointerToID(pl.get()));
        const bool result = StartComponent(label, flags);
        if(result)
        {
            DrawAudioSettings(pl, flags);
            EndComponent();
        }
        ImGui::PopID();
        return result;
    }

    bool Draw(const char* label, const Ref<AudioPlayer3D>& pl, DrawStateFlag flags)
    {
        ImGui::PushID(PointerToID(pl.get()));
        const bool result = StartComponent(label, flags);
        if(result)
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
        }
        ImGui::PopID();
        return result;
    }

    bool Draw(const char* label, const Ref<AudioListener>& ls, DrawStateFlag flags)
    {
        ImGui::PushID(PointerToID(ls.get()));
        const bool result = StartComponent(label, flags);
        if(result)
        {
            auto isEnabled = IsEnabled(ls);
            if (Draw("Enabled", isEnabled, flags)) Enable(ls, isEnabled);
            auto cone = GetCone(ls);
            if(DrawCone(cone, flags)) Set(ls, cone);
            auto velocity = GetVelocity(ls);
            if (Draw("Velocity", velocity, 0, flags | DrawStateFlag::ButtonLess)) SetVelocity(ls, velocity);

            EndComponent();
        }
        ImGui::PopID();
        return result;
    }

    bool DrawCone(AudioCone& cone, DrawStateFlag flags)
    {
        bool result = false;
        result = DrawSlider("Inner Angle", cone.innerAngle, 0.0f, 360.0f, "%.1f", flags) || result;
        result = DrawSlider("Outer Angle", cone.outerAngle, 0.0f, 360.0f, "%.1f", flags) || result;
        result = DrawSlider("Outer Gain", cone.outerGain, 0.0f, 1.0f, "%.1f", flags)     || result;
        result = Draw("Position", cone.position, 0, flags | DrawStateFlag::ButtonLess)            || result;
        result = Draw("Rotation", cone.euler, 0, flags | DrawStateFlag::ButtonLess)               || result;
        return result;
    }

    bool Draw(const char* label, const Ref<AudioGroup>& gr, DrawStateFlag flags)
    {
        ImGui::PushID(PointerToID(gr.get()));
        const bool result = StartComponent(label, flags);
        if(result)
        {
            bool isEnabled = IsEnabled(gr);
            float volume = GetVolume(gr);
            float pitch = GetPitch(gr);

            if (Draw("Enabled", isEnabled, flags)) Enable(gr, isEnabled);
            if (DrawSlider("Volume", volume, 0.0f, 1.0f, "%.2f", flags)) SetVolume(gr, volume);
            if (DrawSlider("Pitch", pitch, 0.0f, 3.0f, "%.2f", flags)) SetPitch(gr, pitch);

            EndComponent();
        }
        ImGui::PopID();
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

    void DrawCameraCore(Camera3DCore& cam, DrawStateFlag flags)
    {
        DrawSlider("Fov", cam.fov, 1.0f, 179.0f, "%.1f", flags);
        Draw("Distance", cam.farPlane, 0.01f, 100000.0f, "%.1f", flags | DrawStateFlag::DontStretchWidth);
        ImGui::SameLine();
        DrawSlider("Near", cam.nearPlane, 0.1f, cam.farPlane, "%.2f", flags | DrawStateFlag::Nameless);
        cam.nearPlane = glm::min(cam.nearPlane, cam.farPlane);

        cam.farPlane = glm::max(cam.nearPlane, cam.farPlane);
    }

    void DrawGridLines(GridLinesInfo& info, bool& enabled, DrawStateFlag flags)
    {
        Draw("Enabled", enabled, flags);
        if (enabled)
        {
            DrawColor("Color", info.color, flags);
            DrawSlider("Thickness", info.lineThickness, 0.01f, 10.0f, "%.2f", flags);
            Draw("Distance", info.drawDistance, 0.01f, 10000.0f, "%.2f", flags);
        }
    }
}