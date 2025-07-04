#include "hyperfloweditor.h"
#include "hyperflow.h"
#include "imgui.h"
#include "imgui_impl_vulkan.h"

namespace hf
{
    static void SetStyle();
    static void SetDarkThemeColors();

    void LoadEditor()
    {
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard |
                          ImGuiConfigFlags_DockingEnable |
                          ImGuiConfigFlags_ViewportsEnable;

        io.ConfigWindowsMoveFromTitleBarOnly = true;
        io.ConfigWindowsResizeFromEdges = true;

        SetDarkThemeColors();
        SetStyle();

        ImGui_ImplVulkan_InitInfo initInfo{};
        ImGui_ImplVulkan_Init(&initInfo);


    }

    void UnloadEditor()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui::DestroyContext();
    }

    void SetStyle()
    {
        ImGuiStyle &style = ImGui::GetStyle();
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        //		style.ItemSpacing = ImVec2(10, 10);
        //		style.ItemInnerSpacing = ImVec2(10, 10);
        style.PopupBorderSize = 2;
        style.SelectableTextAlign = ImVec2(0.5f, 0);
        style.SeparatorTextAlign = ImVec2(0.5f, 0.5f);
        style.SeparatorTextPadding = ImVec2(30, style.FramePadding.y);
    }

    void SetDarkThemeColors()
    {
        auto& colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

        // Headers
        colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Buttons
        colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Frame BG
        colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Tabs
        colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
        colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
        colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

        // Title
        colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    }
}