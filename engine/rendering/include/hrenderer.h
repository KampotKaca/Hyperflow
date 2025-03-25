#ifndef HRENDERER_H
#define HRENDERER_H

#include "hshared.h"

namespace hf
{
	class Renderer
	{
	public:
		Renderer(const Ref<Window>& window);
		~Renderer();
		void* GetGraphicsHandle() const;
		void StartFrame();
		void EndFrame();

	private:
		void* m_GraphicsHandle;
	};
}

#endif //HRENDERER_H
