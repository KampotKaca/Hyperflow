#include "hd3d_graphics.h"
#include "rendering/hrenderer.h"
#include "components/window/hwindow.h"

namespace hf
{
	Renderer::Renderer(Ref<Window> window)
	{
		m_GraphicsHandle = new Graphics((HWND)window->GetHandle());
	}

	Renderer::~Renderer()
	{
		delete (Graphics*)m_GraphicsHandle;
	}
}