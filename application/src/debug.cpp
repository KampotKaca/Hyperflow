#include "debug.h"
#include "application.h"
#include "resources.h"
#include <hyaml.h>

#if DEBUG
#include "hyperfloweditor.h"
#endif

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

    void DebugDefine()
    {

    }

    void DebugLoad()
    {
        
    }

    void DebugStart()
    {
#if DEBUG
        hf::EditorInfo editorInfo
        {
            .windowHandle = GetHandle(hf::GetMainWindow()),
        };
        editorInfo.renderApiHandles = hf::GetEditorApiHandles();
        hf::LoadEditor(editorInfo);
#endif

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

        if (hf::IsKeyDown(hf::Key::V))
        {
        	hf::WindowCreationInfo data =
        	{
        		.title = DEBUG_WINDOW_NAMES[DEBUG_INFO.count % 5],
        		.style = hf::WindowStyle::Default,
        		.position = { 100, 100 },
        		.size = { 200, 200 },
        		.vsyncMode = hf::VsyncMode::Relaxed,
        		.rnEventInfo =
        		{
        			.onPreRenderCallback = AppPreRender,
        			.onRenderCallback = AppRender
        		},
        	};
        	DEBUG_INFO.wn = hf::Create(data, nullptr);
        	DEBUG_INFO.count++;
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

        APP_DEBUG.camera.Update(hf::GetMainWindow(), (float)hf::GetDeltaTime());
        if (hf::IsKeyDown(hf::Key::N)) APP_DEBUG.drawGridLines = !APP_DEBUG.drawGridLines;

        APP_DEBUG.camera.camera3D.core.SetFov(
            APP_DEBUG.camera.camera3D.core.fov -
            hf::GetScrollDelta().y * (float)hf::GetDeltaTime() * 1000.0f);

        if (hf::IsKeyDownContinues(hf::Key::Z))
            APP_OBJECTS.mainLight.direction = glm::rotateX(APP_OBJECTS.mainLight.direction, glm::radians((float)hf::GetDeltaTime() * 25.0f));

        if (hf::IsKeyDownContinues(hf::Key::X))
            APP_OBJECTS.mainLight.direction = glm::rotateX(APP_OBJECTS.mainLight.direction, -glm::radians((float)hf::GetDeltaTime() * 25.0f));
    }

    void DebugQuit()
    {
#if DEBUG
        hf::UnloadEditor();
#endif
    }

    void DebugPreRender(const hf::Ref<hf::Renderer>& rn)
    {
        APP_UNIFORMS.globalUniformInfo.camera = APP_DEBUG.camera.camera3D.GetUniformInfo(rn);
    }

    void DebugPrepass(const hf::Ref<hf::Renderer>& rn)
    {

    }

    void DebugRender(const hf::Ref<hf::Renderer>& rn)
    {
        if (APP_DEBUG.drawGridLines) hf::gridlines::Draw(rn, APP_DEBUG.gridLinesInfo);
    }
}
