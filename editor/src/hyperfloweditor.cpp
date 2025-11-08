#include "heditorinternal.h"
#include "hyperfloweditor.h"
#include "hyperflow.h"
#include "imgui_internal.h"

namespace hf::ed
{
    struct RenderApiInfo
    {
        uint32_t version{};
        VkInstance instance{};
        VkPhysicalDevice physicalDevice{};
        VkDevice device{};
        uint32_t queueFamily{};
        VkQueue queue{};
        VkDescriptorPool descriptorPool{};
        void (*CheckVkResultFn)(VkResult err){};
    };

    struct EditorRenderer
    {
        std::mutex contextLock;
        ImGuiContext* context;
    };

    static EditorRenderer EDITOR_RENDERER{};

    static void SetStyle();
    static void SetDarkThemeColors();

    static RenderApiInfo API_INFO;

    void Load(const LoadInfo& info)
    {
        IMGUI_CHECKVERSION();

        EDITOR_RENDERER.context = ImGui::CreateContext();
        ImGui::SetCurrentContext(EDITOR_RENDERER.context);

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard |
                          ImGuiConfigFlags_DockingEnable |
                          ImGuiConfigFlags_ViewportsEnable;

        io.ConfigWindowsMoveFromTitleBarOnly = true;
        io.ConfigWindowsResizeFromEdges = true;

        SetDarkThemeColors();
        SetStyle();

        ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)info.windowHandle, true);

        API_INFO = *(RenderApiInfo*)info.renderApiHandles;
        ImGui_ImplVulkan_InitInfo initInfo
        {
            .ApiVersion = API_INFO.version,
            .Instance = API_INFO.instance,
            .PhysicalDevice = API_INFO.physicalDevice,
            .Device = API_INFO.device,
            .QueueFamily = API_INFO.queueFamily,
            .Queue = API_INFO.queue,
            .DescriptorPool = API_INFO.descriptorPool,
            .MinImageCount = 3,
            .ImageCount = 3,
            .PipelineCache = VK_NULL_HANDLE,
            .UseDynamicRendering = true,
            .CheckVkResultFn = API_INFO.CheckVkResultFn,
            .MinAllocationSize = 1024 * 1024,
        };
        ImGui_ImplVulkan_Init(&initInfo);
    }

    void Unload()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void StartFrame()
    {
        EDITOR_RENDERER.contextLock.lock();

        ImGui::SetCurrentContext(EDITOR_RENDERER.context);
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        auto viewport = ImGui::GetMainViewport();
        ImGui::DockSpaceOverViewport(viewport->ID, viewport, ImGuiDockNodeFlags_PassthruCentralNode);
    }

    void EndFrame()
    {
        ImGui::Render();
        ImGui::UpdatePlatformWindows();

        EDITOR_RENDERER.contextLock.unlock();
    }

    void DrawFrame(void* cmd)
    {
        std::unique_lock lock(EDITOR_RENDERER.contextLock);

        auto* drawData = ImGui::GetDrawData();
        const bool main_is_minimized = (drawData->DisplaySize.x <= 0.0f ||
                                        drawData->DisplaySize.y <= 0.0f);

        if (!main_is_minimized)
            ImGui_ImplVulkan_RenderDrawData(drawData, (VkCommandBuffer)cmd);
        ImGui::RenderPlatformWindowsDefault();
    }

    void SetStyle()
    {
        ImGuiStyle &style = ImGui::GetStyle();
        auto& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        //		style.ItemSpacing = ImVec2(10, 10);
        //		style.ItemInnerSpacing = ImVec2(10, 10);
        style.IndentSpacing = 7;
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