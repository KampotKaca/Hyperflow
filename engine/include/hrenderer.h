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
		void* drawBufferCache[VULKAN_API_MAX_NUM_DRAW_BUFFER];
	};
}

#endif //HRENDERER_H
