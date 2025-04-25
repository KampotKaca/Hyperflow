#ifndef HRENDERER_H
#define HRENDERER_H

#include "hshared.h"

namespace hf
{
	struct Renderer
	{
		Renderer(uvec2 initialSize);
		Renderer(const Window* window);
		~Renderer();

		uvec2 size{};
		void* windowHandle{};
		void* handle{};
	};
}

#endif //HRENDERER_H
