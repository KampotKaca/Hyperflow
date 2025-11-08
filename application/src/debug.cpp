#include "debug.h"
#include "application.h"
#include "resources.h"

#include "hyperfloweditor.h"
#include "../appconfig.h"

namespace app
{
    AppDebug APP_DEBUG;

    struct DebugInfo
    {
        uint32_t count = 0;
        uint32_t reqCount = 0;
        int32_t lastReq = -1;
    };

    static DebugInfo DEBUG_INFO{};
    const std::string DEBUG_WINDOW_NAMES[5] =
    {
        "N1",
        "N2",
        "N3",
        "N4",
        "N5",
    };

    void DebugRendererInit()
    {
        std::array formats = { hf::TextureFormat::R8G8B8A8_Unorm };
        hf::ed::LoadInfo editorInfo
        {
            .windowHandle = hf::GetWindowHandle(),
            .multisampleMode = MSAA_MODE,
            .pColorFormats = formats.data(),
            .colorFormatCount = formats.size(),
            .depthFormat = DEPTH_STENCIL_MODE
        };
        editorInfo.renderApiHandles = hf::GetEditorApiHandles();
        hf::ed::Load(editorInfo);
    }

    void DebugRendererShutdown()
    {
        hf::ed::Unload();
    }

    void DebugDefine()
    {

    }

    void DebugLoad()
    {

    }

    void DebugStart()
    {
        APP_DEBUG.camera.camera3D.position = hf::vec3{0, 0.5, -2};
        APP_DEBUG.camera.camera3D.direction = hf::vec3{0, 0, 1};
        hf::SetTargetFrameRate(-1);
        DEBUG_INFO.count = 0;
    }

    void DebugUpdate()
    {
        if (hf::IsKeyDown(hf::Key::L))
        {
            auto state = (hf::WindowState)std::max(1, ((int32_t)hf::GetWindowState() + 1) % (int32_t)hf::WindowState::FullscreenBorderless);
            if (state == hf::WindowState::Hidden) state = hf::WindowState::Fullscreen;
            hf::SetWindowState(state);
        }

        if (hf::IsKeyDown(hf::Key::M))
        {
            hf::SetWindowState(hf::WindowState::FullscreenBorderless);
        }

        if (hf::IsKeyDown(hf::Key::B))
        {
            auto state = (hf::WindowPointerState)(((uint32_t)hf::GetWindowPointerState() + 1) % 4);
            hf::SetWindowPointerState(state);
        }

        if (hf::IsKeyDown(hf::Key::K))
        {
            hf::SetVSyncMode((hf::VsyncMode)(((uint32_t)hf::GetVSyncMode() + 1) % (uint32_t)hf::VsyncMode::Count));
        }

        // auto delta = hf::input::GetScrollDelta();
        // if (delta != hf::vec2(0, 0)) LOG_INFO("Scroll (X: %f, Y: %f)", delta.x, delta.y);
        //
        // auto mDelta = hf::input::GetPointerDelta();
        // if (mDelta != hf::ivec2(0, 0)) LOG_INFO("Move: (X: %i, Y: %i)", mDelta.x, mDelta.y);

        auto cReq = (int32_t)(hf::GetTimePassed() / 0.2);
        if (cReq != DEBUG_INFO.reqCount)
        {
            // oss << "[Hyperflow] " << hf::time::GetFrameRate();
            // oss << "[Hyperflow] " << hf::Time::GetTimePassed();
            const std::string str = std::string("[Hyperflow] ") + std::to_string(hf::GetFrameRate());
            hf::SetWindowTitle(str);
            DEBUG_INFO.reqCount = cReq;
        }

        if (hf::IsButtonDownContinues(hf::Button::Right))
            APP_DEBUG.camera.Update((float)hf::GetDeltaTime());

        if (hf::IsKeyDown(hf::Key::N)) APP_DEBUG.drawGridLines = !APP_DEBUG.drawGridLines;
    }

    void DebugQuit()
    {

    }

    void DebugPreRender()
    {
        hf::ed::StartFrame();

        hf::ed::SetNextWindowSize(hf::vec2{ 300, 300 }, hf::ed::Condition::FirstUseEver);
        hf::ed::SetNextWindowPos(hf::vec2{ 300, 300 }, hf::ed::Condition::FirstUseEver);
        if (hf::ed::StartWindow("Project Settings"))
        {
            if (hf::ed::StartDropdown("Lights"))
            {
                hf::ed::Draw("Directional", APP_OBJECTS.mainLight);
                hf::ed::EndDropdown();
            }

            hf::ed::EndWindow();
        }

        hf::ed::DrawMemoryStatisticsWindow("Memory Statistics");
        hf::ed::DrawRenderStatisticsWindow("Rendering Statistics");
        hf::ed::DrawAudioSettingsWindow("Audio Settings");

        hf::ed::EditorContextData data{};
        data.camera = &APP_DEBUG.camera.camera3D.core;
        data.drawGridLines = &APP_DEBUG.drawGridLines;
        data.gridLines = &APP_DEBUG.gridLinesInfo;
        hf::ed::DrawEditorWindow("Editor", data);

        hf::ed::SetNextWindowSize(hf::vec2{ 300, 300 }, hf::ed::Condition::FirstUseEver);
        hf::ed::SetNextWindowPos(hf::vec2{ 100, 100 }, hf::ed::Condition::FirstUseEver);
        if (hf::ed::StartWindow("Inspector"))
        {
            if (hf::ed::StartDropdown("Viking Room"))
            {
                // hf::editor::Draw("Transform", APP_OBJECTS.vikingRoom.transform);
                hf::ed::Draw("Listener", APP_OBJECTS.mainListener);
                hf::ed::EndDropdown();
            }

            if (hf::ed::StartDropdown("Ground"))
            {
                hf::ed::Draw("Transform", APP_OBJECTS.ground.transform);
                hf::ed::EndDropdown();
            }

            if (hf::ed::StartDropdown("Sphere"))
            {
                hf::ed::Draw("Transform", APP_OBJECTS.sphere.transform);
                hf::ed::EndDropdown();
            }

            hf::ed::EndWindow();
        }

        hf::ed::EndFrame();
    }

    void DebugPrepass()
    {

    }

    void GuiDraw(void* cmd)
    {
        hf::ed::DrawFrame(cmd);
    }

    void DebugRender()
    {
        hf::dp::SetDrawCallback(GuiDraw);
    }
}
