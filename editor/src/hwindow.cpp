#include "hyperfloweditor.h"
#include "heditorinternal.h"

namespace hf::editor
{
    bool StartWindow(const char* name, const WindowCreationInfo& info)
    {
        auto result = ImGui::Begin(name, info.pOpen, (ImGuiWindowFlags)info.flags);
        if (result)
        {
            if (info.size.has_value())
            {
                auto size = info.size.value();
                ImGui::SetWindowSize(ImVec2(size.x, size.y));
            }

            if (info.position.has_value())
            {
                auto pos = info.position.value();
                ImGui::SetWindowPos(ImVec2(pos.x, pos.y));
            }
        }

        return result;
    }

    void EndWindow()
    {
        ImGui::End();
    }
}