#include "hd3d_graphics.h"
#include "hrenderer.h"
#include "hwindow.h"
#include "hinternal.h"

namespace hf
{
	Renderer::Renderer(const Ref<Window>& window, const char* version)
	{
		handle = new Graphics((HWND)window->handle);
	}

	Renderer::~Renderer()
	{
		delete (Graphics*)handle;
	}

	namespace inter::rendering
	{
		void StartFrame(Renderer* renderer)
		{
			((Graphics*)renderer->handle)->StartFrame();
		}

		void EndFrame(Renderer* renderer)
		{
			((Graphics*)renderer->handle)->EndFrame();
		}
	}
}