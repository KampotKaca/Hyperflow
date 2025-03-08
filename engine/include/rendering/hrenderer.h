#ifndef HRENDERER_H
#define HRENDERER_H

#include "hshared.h"

namespace hf
{
	enum class RendererState { Stopped, Waiting, Rendering, Dying };
	class Renderer
	{
	public:
		Renderer(Ref<Window> renderingWindow);

		bool Run();

		void StartDrawing();
		void FinishDrawing();

		std::thread* GetThread();
		Ref<Window> GetRenderingWindow();
		RendererState GetState();

	private:
		std::mutex mtx;
		std::condition_variable cv;
		std::atomic<RendererState> m_State;
		std::atomic<int> m_CurrentRender;

		Ref<Window> m_RenderingWindow;
		std::thread m_Thread;

		void Draw();
	};
}

#endif //HRENDERER_H
