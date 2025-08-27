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
        hf::Ref<hf::Window> wn{};
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
        hf::editor::LoadInfo editorInfo
        {
            .windowHandle = GetHandle(hf::GetMainWindow()),
            .multisampleMode = MSAA_MODE,
            .pColorFormats = formats.data(),
            .colorFormatCount = formats.size(),
            .depthFormat = DEPTH_STENCIL_MODE
        };
        editorInfo.renderApiHandles = hf::GetEditorApiHandles();
        hf::editor::Load(editorInfo);
    }

    void DebugRendererShutdown()
    {
        hf::editor::Unload();
    }

    void DebugDefine()
    {

    }

    void DebugLoad()
    {

    }

    void DebugStart()
    {
        APP_DEBUG.camera.camera3D.position = {0, 0.5, -2};
        APP_DEBUG.camera.camera3D.direction = {0, 0, 1};
        DEBUG_INFO.wn = hf::GetMainWindow();
        hf::SetTargetFrameRate(-1);
        DEBUG_INFO.count = 0;
    }

    void DebugUpdate()
    {
        if (hf::IsKeyDown(hf::Key::L))
        {
            auto state = (hf::WindowState)std::max(1, ((int32_t)hf::GetState(DEBUG_INFO.wn) + 1) % (int32_t)hf::WindowState::FullscreenBorderless);
            if (state == hf::WindowState::Hidden) state = hf::WindowState::Fullscreen;
            hf::SetState(DEBUG_INFO.wn, state);
        }

        if (hf::IsKeyDown(hf::Key::M))
        {
            hf::SetState(DEBUG_INFO.wn, hf::WindowState::FullscreenBorderless);
        }

        if (hf::IsKeyDown(hf::Key::B))
        {
            auto state = (hf::WindowPointerState)(((uint32_t)hf::GetPointerState(DEBUG_INFO.wn) + 1) % 4);
            hf::SetPointerState(DEBUG_INFO.wn, state);
        }

        if (hf::IsKeyDown(hf::Key::K))
        {
            hf::SetVSyncMode(DEBUG_INFO.wn, (hf::VsyncMode)(((uint32_t)hf::GetVSyncMode(DEBUG_INFO.wn) + 1) % (uint32_t)hf::VsyncMode::Count));
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
            hf::SetTitle(DEBUG_INFO.wn, str);
            DEBUG_INFO.reqCount = cReq;
        }

        if (hf::IsKeyDown(hf::Key::T))
        {
            switch (hf::GetApiType())
            {
                case hf::RenderingApiType::None:
                    break;
                case hf::RenderingApiType::Vulkan:
                    hf::ChangeApi(hf::RenderingApiType::Direct3D);
                    break;
                case hf::RenderingApiType::Direct3D:
                    hf::ChangeApi(hf::RenderingApiType::Vulkan);
                    break;
            }
        }

        if (hf::IsButtonDownContinues(hf::Button::Right))
            APP_DEBUG.camera.Update(hf::GetMainWindow(), (float)hf::GetDeltaTime());

        if (hf::IsKeyDown(hf::Key::N)) APP_DEBUG.drawGridLines = !APP_DEBUG.drawGridLines;
    }

    void DebugQuit()
    {

    }

    void DebugPreRender(const hf::Ref<hf::Renderer>& rn)
    {
        hf::editor::StartFrame();

        hf::editor::SetNextWindowSize({ 300, 300 }, hf::editor::Condition::FirstUseEver);
        hf::editor::SetNextWindowPos({ 300, 300 }, hf::editor::Condition::FirstUseEver);
        if (hf::editor::StartWindow("Project Settings"))
        {
            if (hf::editor::StartDropdown("Lights"))
            {
                hf::editor::Draw("Directional", APP_OBJECTS.mainLight);
                hf::editor::EndDropdown();
            }

            if (hf::editor::StartDropdown("Cameras"))
            {
                hf::editor::Draw("Editor Camera", APP_DEBUG.camera.camera3D.core);
                hf::editor::EndDropdown();
            }

            hf::editor::EndWindow();
        }

        hf::editor::DrawMemoryStatisticsWindow("Memory Statistics");
        hf::editor::DrawRenderStatisticsWindow("Rendering Statistics");
        hf::editor::DrawAudioSettingsWindow("Audio Settings");

        hf::editor::SetNextWindowSize({ 300, 300 }, hf::editor::Condition::FirstUseEver);
        hf::editor::SetNextWindowPos({ 100, 100 }, hf::editor::Condition::FirstUseEver);
        if (hf::editor::StartWindow("Inspector"))
        {
            if (hf::editor::StartDropdown("Viking Room"))
            {
                // hf::editor::Draw("Transform", APP_OBJECTS.vikingRoom.transform);
                hf::editor::Draw("Audio", APP_AUDIOS.background_music);
                hf::editor::Draw("Audio3D", APP_AUDIOS.background_music3D);
                hf::editor::Draw("Listener", APP_AUDIOS.main_listener);
                hf::editor::EndDropdown();
            }

            if (hf::editor::StartDropdown("Ground"))
            {
                hf::editor::Draw("Transform", APP_OBJECTS.ground.transform);
                hf::editor::EndDropdown();
            }

            if (hf::editor::StartDropdown("Sphere"))
            {
                hf::editor::Draw("Transform", APP_OBJECTS.sphere.transform);
                hf::editor::EndDropdown();
            }

            hf::editor::EndWindow();
        }

        hf::editor::EndFrame();
    }

    void DebugPrepass(const hf::Ref<hf::Renderer>& rn)
    {

    }

    void GuiDraw(const hf::Ref<hf::Renderer>& rn, void* cmd)
    {
        hf::editor::DrawFrame(cmd);
    }

    void DebugRender(const hf::Ref<hf::Renderer>& rn)
    {
        hf::Set_DrawCallback(rn, GuiDraw);
    }
}
