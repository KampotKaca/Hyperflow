#include "exceptions/hgraphicsexception.h"

#include <hwindow.h>

#include "hrenderer.h"
#include "hvk_graphics.h"
#include "hinternal.h"

namespace hf
{
    Renderer::Renderer(const Ref<Window>& window, const char* version)
    {
        if (GRAPHICS_DATA.rendererCount == 0) Graphics_Load(version);
        GRAPHICS_DATA.rendererCount++;

        auto rendererData = new VKRendererData
        {
            .windowHandle = window->handle
        };

        Graphics_LoadPhysicalDevices(rendererData);
        Graphics_LoadSwapchain(rendererData, 0);
        handle = rendererData;
    }

    Renderer::~Renderer()
    {
        const auto data = (VKRendererData*)handle;

        Graphics_UnloadSwapchain(data);
        Graphics_UnloadPhysicalDevices(data);

        delete(data);
        handle = nullptr;

        GRAPHICS_DATA.rendererCount--;
        if (GRAPHICS_DATA.rendererCount == 0) Graphics_Unload();
    }

    namespace inter::rendering
    {
        void StartFrame(Renderer* renderer)
        {

        }

        void EndFrame(Renderer* renderer)
        {

        }
    }
}
