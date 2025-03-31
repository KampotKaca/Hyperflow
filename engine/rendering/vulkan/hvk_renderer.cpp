#include <hwindow.h>

#include "hrenderer.h"
#include "hvk_graphics.h"
#include "exceptions/hgraphicsexception.h"

namespace hf
{
    Renderer::Renderer(const Ref<Window>& window, const char* version)
    {
        if (GRAPHICS_DATA.rendererCount == 0) GraphicsLoad(version);
        GRAPHICS_DATA.rendererCount++;

        auto rendererData = new VKRendererData
        {
            .windowHandle = window->GetHandle()
        };
        m_GraphicsHandle = rendererData;

        VkInstanceCreateInfo createInfo
        {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &GRAPHICS_DATA.appInfo,
            .enabledLayerCount = 0
        };

        VK_HANDLE_EXCEPT(vkCreateInstance(&createInfo, nullptr, &rendererData->instance));
    }

    Renderer::~Renderer()
    {
        auto data = (VKRendererData*)m_GraphicsHandle;
        vkDestroyInstance(data->instance, nullptr);

        delete(data);

        GRAPHICS_DATA.rendererCount--;
        if (GRAPHICS_DATA.rendererCount == 0) GraphicsUnload();
    }

    void Renderer::StartFrame()
    {

    }

    void Renderer::EndFrame()
    {

    }
}
