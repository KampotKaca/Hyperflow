#include "debug.h"
#include "application.h"
#include "resources/renderpasses.h"
#include <hyaml.h>

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

    static hf::RenderPass CreateSubWindowRenderPass(const hf::Ref<hf::Renderer>& rn)
    {
        rn->Bind(APP_RENDER_PASSES.mainPresentPass);
        return APP_RENDER_PASSES.mainPresentPass;
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
        hf::time::SetTargetFrameRate(-1);
        DEBUG_INFO.count = 0;
    }

    void DebugUpdate()
    {
        if (hf::input::IsDown(hf::Key::L))
        {
            auto state = (hf::WindowState)std::max(1, ((int32_t)DEBUG_INFO.wn->GetState() + 1) % (int32_t)hf::WindowState::FullscreenBorderless);
            if (state == hf::WindowState::Hidden) state = hf::WindowState::Fullscreen;
            DEBUG_INFO.wn->SetState(state);
        }

        if (hf::input::IsDown(hf::Key::M))
        {
            DEBUG_INFO.wn->SetState(hf::WindowState::FullscreenBorderless);
        }

        if (hf::input::IsDown(hf::Key::B))
        {
            auto state = (hf::WindowPointerState)(((uint32_t)DEBUG_INFO.wn->GetPointerState() + 1) % 4);
            DEBUG_INFO.wn->SetPointerState(state);
        }

        if (hf::input::IsDown(hf::Key::V))
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
        			.onPassCreationCallback = CreateSubWindowRenderPass,
        			.onPreRenderCallback = AppPreRender,
        			.onRenderCallback = AppRender
        		},
        	};
        	DEBUG_INFO.wn = hf::Window::Open(data, nullptr);
        	DEBUG_INFO.count++;
        }

        if (hf::input::IsDown(hf::Key::K))
        {
            DEBUG_INFO.wn->SetVSyncMode((hf::VsyncMode)(((uint32_t)DEBUG_INFO.wn->GetVSyncMode() + 1) % (uint32_t)hf::VsyncMode::Count));
        }

        // auto delta = hf::input::GetScrollDelta();
        // if (delta != hf::vec2(0, 0)) LOG_INFO("Scroll (X: %f, Y: %f)", delta.x, delta.y);
        //
        // auto mDelta = hf::input::GetPointerDelta();
        // if (mDelta != hf::ivec2(0, 0)) LOG_INFO("Move: (X: %i, Y: %i)", mDelta.x, mDelta.y);

        auto cReq = (int32_t)(hf::time::GetTimePassed() / 0.2);
        if (cReq != DEBUG_INFO.reqCount)
        {
            // oss << "[Hyperflow] " << hf::time::GetFrameRate();
            // oss << "[Hyperflow] " << hf::Time::GetTimePassed();
            const std::string str = std::string("[Hyperflow] ") + std::to_string(hf::time::GetFrameRate());
            DEBUG_INFO.wn->SetTitle(str);
            DEBUG_INFO.reqCount = cReq;
        }

        if (hf::input::IsDown(hf::Key::T))
        {
            switch (hf::Renderer::GetApiType())
            {
                case hf::RenderingApiType::None:
                    break;
                case hf::RenderingApiType::Vulkan:
                    hf::Renderer::ChangeApi(hf::RenderingApiType::Direct3D);
                    break;
                case hf::RenderingApiType::Direct3D:
                    hf::Renderer::ChangeApi(hf::RenderingApiType::Vulkan);
                    break;
            }
        }

        APP_DEBUG.camera.Update(hf::GetMainWindow(), (float)hf::time::GetDeltaTime());
        if (hf::input::IsDown(hf::Key::N)) APP_DEBUG.drawGridLines = !APP_DEBUG.drawGridLines;
    }

    void DebugQuit()
    {

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
