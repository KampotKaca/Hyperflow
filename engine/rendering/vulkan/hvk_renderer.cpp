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
        Graphics_LoadSurface(rendererData);
        m_GraphicsHandle = rendererData;
    }

    Renderer::~Renderer()
    {
        const auto data = (VKRendererData*)m_GraphicsHandle;

        Graphics_UnloadPhysicalDevices(data);
        Graphics_UnloadSurface(data);

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
