#include "debug.h"
#include "application.h"
#include "resources/renderpasses.h"
#include <hyaml.h>

namespace app
{
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
        hf::renderpass::Bind(rn, APP_RENDER_PASSES.mainPresentPass);
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
        DEBUG_INFO.wn = hf::GetMainWindow();
        hf::time::SetTargetFrameRate(-1);
        DEBUG_INFO.count = 0;
    }

    void DebugUpdate()
    {
        // if (hf::input::IsDown(hf::Key::A))
        // {
        // 	hf::WindowCreationInfo data =
        // 	{
        // 		.title = DEBUG_WINDOW_NAMES[DEBUG_INFO.count % 5],
        // 		.flags = hf::WindowFlags::Default,
        // 		.style = hf::WindowStyle::Default,
        // 		.position = { 100, 100 },
        // 		.size = { 200, 200 },
        // 		.vsyncMode = hf::VsyncMode::Relaxed,
        // 		.rnEventInfo =
        // 		{
        // 			.onPassCreationCallback = CreateSubWindowRenderPass,
        // 			.onPreRenderCallback = AppPreRender,
        // 			.onRenderCallback = AppRender
        // 		},
        // 	};
        // 	DEBUG_INFO.wn = hf::window::Open(data, nullptr);
        // 	DEBUG_INFO.count++;
        // }
        //
        // if (hf::input::IsDown(hf::Key::Space) && DEBUG_INFO.wn)
        // {
        //     hf::window::Close(DEBUG_INFO.wn);
        // }

        if (hf::input::IsDown(hf::Key::K))
        {
            hf::window::SetVSyncMode(DEBUG_INFO.wn, (hf::VsyncMode)(((uint32_t)hf::window::GetVSyncMode(DEBUG_INFO.wn) + 1) % (uint32_t)hf::VsyncMode::Count));
        }

        // auto delta = hf::input::GetScrollDelta();
        // if (delta != hf::vec2(0, 0)) LOG_INFO("Scroll (X: %f, Y: %f)", delta.x, delta.y);
        //
        // auto mDelta = hf::input::GetPointerDelta();
        // if (mDelta != hf::ivec2(0, 0)) LOG_INFO("Move: (X: %i, Y: %i)", mDelta.x, mDelta.y);

        auto cReq = (int32_t)(hf::time::GetTimePassed() / 0.2);
        if (cReq != DEBUG_INFO.reqCount)
        {
            // std::ostringstream oss;
            // oss << "[Hyperflow] " << hf::time::GetFrameRate();
            // oss << "[Hyperflow] " << hf::Time::GetTimePassed();
            std::string str = std::string("[Hyperflow] ") + std::to_string(hf::time::GetFrameRate());
            hf::window::SetTitle(hf::GetMainWindow(), str);
            DEBUG_INFO.reqCount = cReq;
        }

        if (hf::input::IsDown(hf::Key::T))
        {
            switch (hf::renderer::GetApiType())
            {
                case hf::RenderingApiType::None:
                    break;
                case hf::RenderingApiType::Vulkan:
                    hf::renderer::ChangeApi(hf::RenderingApiType::Direct3D);
                    break;
                case hf::RenderingApiType::Direct3D:
                    hf::renderer::ChangeApi(hf::RenderingApiType::Vulkan);
                    break;
            }
        }
    }

    void DebugQuit()
    {

    }

    void DebugPreRender(const hf::Ref<hf::Renderer>& rn)
    {

    }

    void DebugRender(const hf::Ref<hf::Renderer>& rn)
    {

    }
}
