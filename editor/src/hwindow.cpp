#include "hyperfloweditor.h"
#include "heditorinternal.h"
#include "hwindow.h"

namespace hf::editor
{
    bool StartWindow(const char* name, bool* isOpen, WindowFlags flags) { return ImGui::Begin(name, isOpen, (ImGuiWindowFlags)flags | ImGuiWindowFlags_DockNodeHost); }
    void EndWindow() { ImGui::End(); }

    void SetNextWindowSize(vec2 size, Condition cond) { ImGui::SetNextWindowSize({ size.x, size.y }, (ImGuiCond)cond); }
    void SetNextWindowPos(vec2 pos, Condition cond) { ImGui::SetNextWindowPos({ pos.x, pos.y }, (ImGuiCond)cond); }
}