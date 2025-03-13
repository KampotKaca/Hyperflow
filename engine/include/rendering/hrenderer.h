#ifndef HRENDERER_H
#define HRENDERER_H

#include "hshared.h"

namespace hf
{
	class Renderer
	{
	public:
		Renderer(Ref<Window> window);
		~Renderer();
		void* GetGraphicsHandle();

	private:
		void* m_GraphicsHandle;
	};
}

#endif //HRENDERER_H
