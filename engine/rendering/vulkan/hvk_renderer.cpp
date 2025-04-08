#include "exceptions/hgraphicsexception.h"

#include <hwindow.h>

#include "hrenderer.h"
#include "hvk_graphics.h"

namespace hf
{
    Renderer::Renderer(const Ref<Window>& window, const char* version)
    {
        if (GRAPHICS_DATA.rendererCount == 0) Graphics_Load(version);
        GRAPHICS_DATA.rendererCount++;

        auto rendererData = new VKRendererData
        {
            .windowHandle = window->GetHandle()
        };

        Graphics_LoadPhysicalDevices(rendererData);
        Graphics_LoadSwapchain(rendererData, 0);
        m_GraphicsHandle = rendererData;
    }

    Renderer::~Renderer()
    {
        const auto data = (VKRendererData*)m_GraphicsHandle;

        Graphics_UnloadSwapchain(data);
        Graphics_UnloadPhysicalDevices(data);

        delete(data);
        m_GraphicsHandle = nullptr;

        GRAPHICS_DATA.rendererCount--;
        if (GRAPHICS_DATA.rendererCount == 0) Graphics_Unload();
    }

    void Renderer::StartFrame()
    {

    }

    void Renderer::EndFrame()
    {

    }
}
