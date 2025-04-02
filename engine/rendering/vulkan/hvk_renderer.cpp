#include <hunsupportedexception.h>
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
            .enabledLayerCount = 0,
            .enabledExtensionCount = (uint32_t)REQUIRED_EXTENSIONS.size(),
            .ppEnabledExtensionNames = REQUIRED_EXTENSIONS.data(),
        };

#if DEBUG
        {
            const char* validationLayers[] = { "VK_LAYER_KHRONOS_validation" };

            for (const char* layer : validationLayers)
            {
                if (!GraphicsValidateLayerSupport(layer))
                    throw UNSUPPORTED_EXCEPT(layer);
            }

            createInfo.ppEnabledLayerNames = validationLayers;
            createInfo.enabledLayerCount = 1;
        }
#endif

        VK_HANDLE_EXCEPT(vkCreateInstance(&createInfo, nullptr, &rendererData->instance));
    }

    Renderer::~Renderer()
    {
        const auto data = (VKRendererData*)m_GraphicsHandle;

        vkDestroyInstance(data->instance, nullptr);
        delete(data);
        m_GraphicsHandle = nullptr;

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
