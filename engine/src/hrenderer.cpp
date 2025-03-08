#include "hrenderer.h"

namespace hf
{
	Renderer::Renderer(Ref<Window> renderingWindow)
	{
		m_RenderingWindow = renderingWindow;
	}

	std::thread* Renderer::GetThread() { return &m_Thread; }
	Ref<Window> Renderer::GetRenderingWindow() { return m_RenderingWindow; }
	RendererState Renderer::GetState() { return m_State; }

	void Renderer::StartDrawing()
	{

	}

	void Renderer::FinishDrawing()
	{

	}

	bool Renderer::Run()
	{

	}
}