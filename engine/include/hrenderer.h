#ifndef HRENDERER_H
#define HRENDERER_H

#include "hshared.h"
#include "../config.h"

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
		void* vertBufferCache[MAX_NUM_BUFFER_CACHE]{};
	};
}

#endif //HRENDERER_H
