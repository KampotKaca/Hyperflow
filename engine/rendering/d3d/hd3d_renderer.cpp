#include "hd3d_graphics.h"
#include "hrenderer.h"
#include "hwindow.h"

namespace hf
{
	Renderer::Renderer(const Ref<Window>& window)
	{
		m_GraphicsHandle = new Graphics((HWND)window->GetHandle());
	}

	Renderer::~Renderer()
	{
		delete (Graphics*)m_GraphicsHandle;
	}

	void Renderer::StartFrame()
	{
		((Graphics*)m_GraphicsHandle)->StartFrame();
	}

	void Renderer::EndFrame()
	{
		((Graphics*)m_GraphicsHandle)->EndFrame();
	}
}