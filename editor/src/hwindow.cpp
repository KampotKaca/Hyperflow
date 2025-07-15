#include "hyperfloweditor.h"
#include "heditorinternal.h"

namespace hf::editor
{
    bool StartWindow(const char* name, bool* isOpen, WindowFlags flags)
    {
        ImGuiWindowClass window_class;
        window_class.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoAutoMerge;
        ImGui::SetNextWindowClass(&window_class);

        bool result = ImGui::Begin(name, isOpen, (ImGuiWindowFlags)flags);
        if (!result) EndWindow();
        return result;
    }
    void EndWindow() { ImGui::End(); }

    void SetNextWindowSize(vec2 size, Condition cond) { ImGui::SetNextWindowSize({ size.x, size.y }, (ImGuiCond)cond); }
    void SetNextWindowPos(vec2 pos, Condition cond) { ImGui::SetNextWindowPos({ pos.x, pos.y }, (ImGuiCond)cond); }
}