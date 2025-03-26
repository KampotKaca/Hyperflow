#include <hwindow.h>

#include "hrenderer.h"
#include "hvk_graphics.h"

namespace hf
{
    Renderer::Renderer(const Ref<Window>& window)
    {
        m_GraphicsHandle = new VKGraphics(window->GetHandle());
    }

    Renderer::~Renderer()
    {
        delete((VKGraphics*)m_GraphicsHandle);
    }

    void Renderer::StartFrame()
    {

    }

    void Renderer::EndFrame()
    {

    }
}
